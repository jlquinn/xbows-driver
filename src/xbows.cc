#include <iostream>
#include <stdexcept>
#include <vector>
#include <unistd.h>

#include "packet.hh"
#include "timer.hh"
#include "driver_parse.hh"

using namespace std;

#define REALRUN

// 1ea7:0907  
unsigned int xbows_vendor = 0x1ea7;
unsigned int xbows_product = 0x0907;


void hiderror(const string& s) {
  cerr << "ERROR: " << s << endl;
  exit(-1);
}

void dump_packet(const unsigned char* data) {
  cout << "Packet:\n";
  for (int i=0; i < 64; i++) {
    printf(" %02x", data[i]);
    if ((i+1) % 16 == 0) printf("\n");
  }
  cout << flush;
}


// 1 millisecond delay
void do_interval() {
  // struct timespec ms = {0, 1000000}; // 1 millisec = 1M nanosec
  // nanosleep(&ms, nullptr);
  usleep(1000);
}

// Don't send empty sequences and expect nothing to be sent
void send_packets(hid_device* dev, bool suppress, const vector<packet>& prog,
		  unsigned start=0, unsigned end=0) {
  // OK, we have a handle to the xbows programming interface.
  packet data;

  cout << "Sending packets now..." << endl;
  timestamp t = now();
  for (end = !end ? prog.size() : end; start < end; start++) {
    const auto& pkt = prog[start];
  // for (const auto& pkt: prog) {
    //// Send and receive xbows light program command
    cout << "\nSENDING PACKET\n";
    if (pkt.delay) usleep(pkt.delay * 1000);
    if (!suppress) {
#ifdef REALRUN
      int numwr = hid_write(dev, pkt.bytes, 64);
      if (numwr == -1) {
	hid_close(dev);
	hiderror("Failed to write data to hid device");
      }
#endif
    }
    cout << pkt.to_string();
    do_interval();
    cout << "\nRECEIVING PACKET"<<endl;
    if (!suppress) {
#ifdef REALRUN
      int numrd = hid_read(dev, data.bytes, 64);
      if (numrd != 64) {
	hid_close(dev);
	hiderror("Failed to read data from hid device");
      }
      cout << data.to_string();
#endif
    }
    do_interval();
  }
  cout << "Sending light program DONE" << endl;
  timestamp t2 = now();
  cout << "Sent program in " << (t2-t).millis() << " ms\n";
}


// Select the HID device for the XBows keyboard

// XXX for now selects the first xbows keyboard that matches The hid library
// will return a list of device/interface objects.  XBows has 5 usb interfaces
// in the device.  If there is more than one keyboard, I assume there will be
// 5 per keyboard.  We'll find only the first one we see.
string find_device(unsigned short vendor, unsigned short product) {
  // Find the right interface
  struct hid_device_info* devlist = hid_enumerate(vendor, product);
  if (devlist == nullptr)
    hiderror("Failed to enumerate xbows keyboard device");

  // Print the various keyboard bits
  int devcount = 0;
  for (struct hid_device_info* devp = devlist; devp != nullptr;
       devp = devp->next) {
    cout << "DEVICE:\n";
    cout << "path:	" << devp->path << "\n";
    cout << "vendprod:  " << devp->vendor_id << " " << devp->product_id << "\n";
    cout << "release:   " << devp->release_number << "\n";
    // These are wchar_t strings
    wcout << "mfgstr:   " << devp->manufacturer_string << "\n";
    wcout << "prodstr:  " << devp->product_string << "\n";
    cout << "usagepg:   " << devp->usage_page << "\n";
    cout << "usage:     " << devp->usage << "\n";
    cout << "interface: " << devp->interface_number << "\n";
    devcount++;
  }

  string progpath;
  for (struct hid_device_info* devp = devlist; devp != nullptr;
       devp = devp->next) {
    if (devp->interface_number == 1)
      progpath = devp->path;
  }  

  hid_free_enumeration(devlist);

  if (progpath == "")
    hiderror("Failed to find xbows interface 1");

  if (devcount > 3)
    hiderror("I'm confused - I found more than one xbows keyboard");

  return progpath;
}


hid_device* open_keyboard() {
  string progpath = find_device(xbows_vendor, xbows_product);

  // At this point I could try to read the interface and endpoint descriptors,
  // but I already know what they are and don't need anything from them.  I
  // think.  There is no useful usage or usage page, so the HID protocol is
  // just a carrier for communicating with the keyboard.

  hid_device* dev = hid_open_path(progpath.c_str());
  if (dev == nullptr)
    hiderror("Failed to open hid device at " + progpath);


  // XXX Here we should query the xbows info such as firmware

  return dev;
}


void close_keyboard(hid_device* dev) {
  hid_close(dev);
  
  if (hid_exit())
    hiderror("Failed to close hidapi library");
}

xbows_state init_xbows() {
  init_packet();
  init_parse();
  hid_device* dev = open_keyboard();
  xbows_state state;
  state.dev = dev;
  state.suppress = false;
  state.layer = DRIVER;
  state.drv_frame = 0;
  state.lastcmd = now();
  return state;
}

void xbows_close(xbows_state& state) {
  close_keyboard(state.dev);
}


bool xbows_send(xbows_state* state, program& prog, layercode layer) {
  send_packets(state->dev, state->suppress, drv_idle);
  // First convert prog to packet sequences

  // Use previous layer if not provided by parameter or program.
  if (layer == NONE) {
    layer = prog.layer;
    if (layer == NONE)
      layer = state->layer;
  }

  // Driver layer
  if (layer == DRIVER) {
    // convert kmap to sequence
    state->drv_kmap = driver_keymap_program(prog.kmap);
    state->drv_kmap_default = prog.kmap.is_default();

    // convert light prog to frame sequence
    state->drv_lights = driver_light_program(prog.lights);

    // Switch keyboard to driver layer and get its attention
    send_packets(state->dev, state->suppress, drv_idle);
    send_packets(state->dev, state->suppress, drv_attn);
    
    
    // Send keymap
    send_packets(state->dev, state->suppress, state->drv_kmap);

    // send first light frame
    return xbows_update(state);
  }

  // Custom layer 2 3 4
  if (layer >= CUSTOM1 && layer <= CUSTOM3) {
    // convert kmap to sequence
    // convert light program to sequence
    state->cust1 = custom_program(layer, prog);
    send_packets(state->dev, state->suppress, state->cust1);
    send_packets(state->dev, state->suppress, drv_idle);
  }

  // Bad layer
  return false;
}


// Send next frame of driver animation.
bool xbows_update(xbows_state* state) {
  cout << "xbows_update\n" << flush;
  if (state->drv_lights.empty() && state->drv_kmap_default) {
    send_packets(state->dev, state->suppress, drv_idle);
    cout << "idle\n" << flush;
    usleep(300000);
    return false;
  }

  // send next frame
  if (state->drv_lights.size()) {
    cout << "update lights\n" << flush;
    send_packets(state->dev, state->suppress,
		 state->drv_lights, state->drv_frame,
		 state->drv_frame + drv_frame_packets);

    // Advance counter and timestamp.  Driver light frame is 13 packets.
    state->drv_frame = (state->drv_frame+drv_frame_packets) % state->drv_lights.size();
  }
  else
    cout << "lights " << state->drv_lights.size() << endl;

  // Refresh keymap if needed (if not the default)
  timestamp n = now();
  timestamp gap = (n - state->lastcmd);
  if (state->drv_kmap_default==false && gap.millis() > 200) {
    cout << "Updating keymap\n" << flush;
    send_packets(state->dev, state->suppress, drv_idle);
    send_packets(state->dev, state->suppress, drv_attn);
    send_packets(state->dev, state->suppress, state->drv_kmap);
    state->lastcmd = now();
  }
  else
    cout << "kmap dflt " << state->drv_kmap_default << " gap " << gap.millis() << endl;

  return true;
}


void xbows_factory_reset(xbows_state* state) {
  // Get keyboard attention
  send_packets(state->dev, state->suppress, drv_idle);

  vector<packet> reset(1, packet(0x0b, 0x02));
  reset.back().compute_crc();
  send_packets(state->dev, state->suppress, reset);
}


void xbows_switch_layer(xbows_state* state, layercode layer) {
  if (layer < CUSTOM1 || layer > DRIVER)
    throw runtime_error("Attempt to switch to invalid layer - must be DRIVER or CUSTOM1-3\n");
  // Get keyboard attention
  send_packets(state->dev, state->suppress, drv_idle);

  vector<packet> reset(1, packet(0x0b, layer));
  reset.back().compute_crc();
  send_packets(state->dev, state->suppress, reset);
}
