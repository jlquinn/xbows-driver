#ifndef XBOWS_HH
#define XBOWS_HH

#include <string>
#include <vector>

#include <hidapi.h>
#include "packet.hh"
#include "driver_layer.hh"
#include "custom_layer.hh"
#include "timer.hh"

// Combination of custom layer program and driver layer programming.
//
// The user can create this programmatically, or by parsing yaml file
struct program {
  keymap kmap;		// driver or custom.
  rgb_lights lights;	// driver only
  custom_light_prog custom_lights; // custom only?  driver seems to support this
  // We can have multiple flashlight progs?
  custom_light_prog flashlight; // custom only?  driver seems to support this
};

// Converts prog into a sequence of packets targeted at layer.
std::vector<packet> convert_program(char layer, program& prog);

struct xbows_state {
  hid_device* dev;		// keyboard device
  int suppress;			// suppress sending packets to device for testing
  int layer;			// layer keyboard will work on. this may be
				// different than the real kbd but the next
				// xbows_update() will switch to this layer.
  
  std::vector<packet> drv_kmap;	// driver keymap program
  std::vector<packet> drv_lights; // driver light program
  int drv_frame;		// last driver frame sent to keyboard
  timestamp lastcmd;		// how long since last command was sent
  bool drv_kmap_sent;		// if we haven't timed out, we can send the
				// keymap once and keep sending light frames
 
  std::vector<packet> cust1;	// last program for cust1 layer
  std::vector<packet> cust2;	// last program for cust1 layer
  std::vector<packet> cust3;	// last program for cust1 layer

  uint32_t firmware_id;
  uint32_t firmware_ver;

  // Basic HID device info
  std::string path;
  std::string vendor_id;
  std::string product_id;
  std::string release;
  std::string manufacturer;
  int interface_number;
};


xbows_state init_xbows();

void xbows_close(xbows_state& state);


// Send program to keyboard
// XXX HOW DOES LAYER GET SELECTED?
// If layer is specified, send to that layer.  Else send to layer selected in
// switch_layer.  No way to query kbd what layer we're on as far as I know.
// if program is clearly driver or custom we should use that
// Return true if update should be called to send more packets
bool xbows_send(xbows_state* state, program& prog, int layer=-1);


// Take the next step for layer.  Switch to the layer

// For driver mode, sends next step.  Must be called every 3 sec or more
// often, or keyboard will revert to stock settings
// If there's nothing new to send, send an idle command

// For custom mode, update the layer with the program in state.

// Return true if update should be called again to send more packets -
// normally driver mode rgb light frames.
bool xbows_update(xbows_state* state);

void xbows_factory_reset(xbows_state* state);

// Tell keyboard to use layer
void xbows_switch_layer(xbows_state* state, int layer);


#endif