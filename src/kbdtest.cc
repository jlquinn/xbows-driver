// Simple test program to talk to xbows keyboard using hidapi

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>

#include <checksum.h>
#include <hidapi.h>

using namespace std;

void dump_packet(unsigned char* data) {
  cout << "Packet:\n";
  for (int i=0; i < 64; i++) {
    printf(" %02x", data[i]);
    if ((i+1) % 16 == 0) printf("\n");
  }
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
  unsigned char data[64];
  memset(data, 0, 64);
  data[0] = 0x0c;
  data[6] = 0xa7;
  data[7] = 0x0d;


  //// Send and receive xbows idle command
  int numwr = hid_write(dev, data, 64);
  if (numwr == -1) {
    hid_close(dev);
    hiderror("Failed to write data to hid device");
  }
  int numrd = hid_read(dev, data, 64);
  if (numrd != 64) {
    hid_close(dev);
    hiderror("Failed to read data from hid device");
  }

  dump_packet(data);

  //// Send a basic driver mode light program ////
  // TODO
  // Convert a program to a sequence of packets
  // Send the packet sequence to the keyboard.
  // Each program is going to be somewhat different.
  // I need to represent a program, and the packet sequence.

  // Let's try sending the green calculator program to the keyboard in driver
  // mode
  
  

  
  hid_close(dev);
  
  if (hid_exit())
    hiderror("Failed to close hidapi library");
  
  return 0;
}
