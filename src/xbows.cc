#include <iostream>
#include <stdexcept>
#include <vector>
#include <unistd.h>

#include "packet.hh"
#include "timer.hh"
#include "driver_parse.hh"

using namespace std;

#define REALRUN


// Converts prog into a sequence of packets targeted at layer.
// layer 1-3 are custom, 0 is driver
vector<packet> convert_program(unsigned char layer, program& prog) {
  vector<packet> pkts;

  if (layer > 3)
    throw runtime_error("Bad layer not 0-3");
  
  if (layer == 0)
    return driver_light_program(prog.lights);

  // XXX does nothing right now
  if (layer == 0)
    return driver_keymap_program(prog.kmap);

  // Error checking here
  if (layer >= 1 && layer <= 3) {
    if (prog.lights.size())
      throw runtime_error("Attempt to program custom layer with rgb frames");
    
    return custom_program(layer, prog);
  }
  
  return pkts;
}


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

// Don't send empty subprograms and expect nothing to be sent
void send_program(hid_device* dev, bool suppress, const vector<packet>& prog,
		  unsigned start=0, unsigned end=0) {
  // OK, we have a handle to the xbows programming interface.
  packet data;

  cout << "Sending program now..." << endl;
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
  state.layer = 1;
  state.drv_frame = 0;
  state.lastcmd = now();
  return state;
}

void xbows_close(xbows_state& state) {
  close_keyboard(state.dev);
}


bool xbows_send(xbows_state* state, program& prog, int layer) {
  send_program(state->dev, state->suppress, drv_idle);
  // First convert prog to packet sequences

  // Use last layer if not provided
  if (layer == -1)
    layer = state->layer;

  // Driver layer
  if (layer == 1) {
    // convert kmap to sequence
    state->drv_kmap = driver_keymap_program(prog.kmap);

    // convert light prog to frame sequence
    state->drv_lights = driver_light_program(prog.lights);

    // Switch keyboard to driver layer and get its attention
    send_program(state->dev, state->suppress, drv_idle);
    send_program(state->dev, state->suppress, drv_attn);
    
    
    // Send keymap
    send_program(state->dev, state->suppress, state->drv_kmap);

    // send first light frame
    return xbows_update(state);
  }

  // Custom layer 2 3 4
  if (layer > 1 && layer < 5) {

    
  }

  // Bad layer
  return false;
}


// Send next frame of driver animation.
bool xbows_update(xbows_state* state) {
  if (state->drv_lights.empty()) {
    send_program(state->dev, state->suppress, drv_idle);
    return false;
  }

  // send next frame
  send_program(state->dev, state->suppress,
	       state->drv_lights, state->drv_frame,
	       state->drv_frame + drv_frame_packets);

  // Advance counter and timestamp.  Driver light frame is 13 packets.
  state->drv_frame = (state->drv_frame+drv_frame_packets) % state->drv_lights.size();
  state->lastcmd = now();

  return true;
}


void xbows_factory_reset(xbows_state* state) {
  // Get keyboard attention
  send_program(state->dev, state->suppress, drv_idle);

  vector<packet> reset(1, packet(0x0b, 0x02));
  reset.back().compute_crc();
  send_program(state->dev, state->suppress, reset);
}
