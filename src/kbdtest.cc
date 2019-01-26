// Simple test program to talk to xbows keyboard using hidapi

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

#include <checksum.h>
#include <hidapi.h>

#include "driver_layer.hh"
#include "timer.hh"

using namespace std;

#define REALRUN 1


void dump_packet(const unsigned char* data) {
  cout << "Packet:\n";
  for (int i=0; i < 64; i++) {
    printf(" %02x", data[i]);
    if ((i+1) % 16 == 0) printf("\n");
  }
  cout << flush;
}

void hiderror(const string& s) {
  cerr << "ERROR: " << s << endl;
  exit(-1);
}

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

void do_interval() {
  // struct timespec ms = {0, 1000000}; // 1 millisec = 1M nanosec
  // nanosleep(&ms, nullptr);
  usleep(1000);
}

void send_program(hid_device* dev, const vector<packet>& prog) {
  // OK, we have a handle to the xbows programming interface.
  packet data;

  cout << "Sending program now..." << endl;
  timestamp t = now();
  for (const auto& pkt: prog) {
    //// Send and receive xbows light program command
    cout << "\nSENDING PACKET\n";
#ifdef REALRUN
    int numwr = hid_write(dev, pkt.bytes, 64);
    if (numwr == -1) {
      hid_close(dev);
      hiderror("Failed to write data to hid device");
    }
#endif
    cout << pkt.to_string();
    do_interval();
    cout << "\nRECEIVING PACKET"<<endl;
#ifdef REALRUN
    int numrd = hid_read(dev, data.bytes, 64);
    if (numrd != 64) {
      hid_close(dev);
      hiderror("Failed to read data from hid device");
    }
    cout << data.to_string();
#endif
    do_interval();
  }
  cout << "Sending light program DONE" << endl;
  timestamp t2 = now();
  cout << "Sent program in " << (t2-t).millis() << " ms\n";
}


int main(int ac, char* av[]) {

  // 1ea7:0907  
  unsigned int vendor = 0x1ea7;
  unsigned int product = 0x0907;
  
  if (hid_init())
    hiderror("Failed to init hidapi library");

  string progpath = find_device(vendor, product);

  // At this point I could try to read the interface and endpoint descriptors,
  // but I already know what they are and don't need anything from them.  I
  // think.  There is no useful usage or usage page, so the HID protocol is
  // just a carrier for communicating with the keyboard.

  hid_device* dev = hid_open_path(progpath.c_str());
  if (dev == nullptr)
    hiderror("Failed to open hid device at " + progpath);

  // OK, we have a handle to the xbows programming interface.
  packet data(0x0c, 00);
  data.compute_crc();


  //// Send and receive xbows idle command
  int numwr = hid_write(dev, data.bytes, 64);
  if (numwr == -1) {
    hid_close(dev);
    hiderror("Failed to write data to hid device");
  }
  do_interval();
  int numrd = hid_read(dev, data.bytes, 64);
  if (numrd != 64) {
    hid_close(dev);
    hiderror("Failed to read data from hid device");
  }
  do_interval();

  cout << data.to_string();

  //// Send a basic driver mode light program ////
  // TODO
  // Convert a program to a sequence of packets
  // Send the packet sequence to the keyboard.
  // Each program is going to be somewhat different.
  // I need to represent a program, and the packet sequence.

  // Let's try sending the green calculator program to the keyboard in driver
  // mode


  // XXX test driver light program
  //  vector<drv_light_frame> calc = make_calc();
  // vector<drv_light_frame> calc = make_trail();
  // vector<packet> lgtprog = light_program(calc);

  // XXX test driver keymap program
  // Start with a default keymap
  drv_keymap kmap;
  kmap.assign(K_Z, K_Q);
  
  vector<packet> kprog = keymap_program(kmap);

  send_program(dev, kprog);

  // sending calc program clobbers keymap
  sleep(2);
  // vector<packet> calc = light_program(make_calc());
  vector<packet> idle;
  idle.push_back(packet(0x0c, 0));
  idle.back().compute_crc();
  
  for (int i=0; i < 10; i++) {
    send_program(dev, idle);
    sleep(2);
  }

  
  hid_close(dev);
  
  if (hid_exit())
    hiderror("Failed to close hidapi library");
  
  return 0;
}
