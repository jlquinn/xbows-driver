// Control Xbows driver layer.
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include <endian.h>

#include "driver_layer.hh"
#include "keymap.hh"
#include "packet.hh"

using namespace std;

drv_light_frame::drv_light_frame() {
  memset(keys, 0, sizeof(keys));
}

// RGB is 3 bytes with top byte = 00.  Red is lowest byte, Blue is highest.
void drv_light_frame::setkey(keycodes key, uint32_t rgb) {
  assert((rgb & 0xff000000) == 0);
  int pos = drv_light_assign[key];
  keys[pos] = rgb | 0x64000000;
}

void drv_light_frame::clear() {
  memset(keys, 0, sizeof(keys));
}


vector<packet> drv_attn;
void init_driver_mode() {
  if (drv_attn.empty()) {
    // Initial program packets.  This gets the keyboard's attention.
    drv_attn.push_back(packet(0x0b, 0x05));
    drv_attn.push_back(packet(0x01, 0x09));

    // Compute crc for each packet
    for (auto& pkt: drv_attn)
      pkt.compute_crc();
  }
}


// Generate packets for a driver mode light program.  To do this, I need to
// assign each key an RGB value.  The return needs to contain the full packet
// sequence, as well as expected return codes from the keyboard to make sure
// nothing is going wrong.
vector<packet> light_program(const vector<drv_light_frame>& framelist) {
  // For each key, we want an RGB value.  Convert to a sequence of packets.
  // Sequence must start with 0x0b05
  // Sequence must next have 0x0109
  init_driver_mode();

  // Driver mode light program is 10 packets.
  vector<packet> program;


  // Convert to packet program

  // Get the keyboard's attention.
  program.assign(drv_attn.begin(), drv_attn.end());


  for (const auto& frame: framelist) {
  
    // Light program.  14 key rgb per packet
    int remaining = frame.size();
    for (int i=0; i < frame.size(); i+=14, remaining-=14) {
      packet pkt(0x1a, 0x01);

      // Number of bytes already added to the program
      pkt.progcount = htole16(i * 4);

      // Store bytes of packet data in byte 5
      unsigned char pktbytes = min(remaining * 4, 56);
      pkt.datasize = pktbytes;

      // Copy the packet data
      for (int j=0; j < min(remaining, 14); j++) {
	uint32_t val = htole32(frame.keys[i+j]);
	memcpy(pkt.data+j*4, &val, 4);
      }
      program.push_back(pkt);
    }

    // Terminator packet
    program.push_back(packet(0x1a, 0x02));
  }

  // assert(program.size() == 11);

  // Compute crc for each packet
  for (auto& pkt: program)
    pkt.compute_crc();

  return program;
}

int drv_light_assign[MAX_KEYCODE] = {
  // Position 0 is reserved
  0xff,				// K_NONE
  // Letters
  67, 94, 92, 70, 48, 71, 72, 74, // A B C D E F G H
  54, 75, 76, 78, 97, 96, 56, 57, // I J K L M N O P
  45, 49, 68, 50, 53, 93, 46, 90, // Q R S T U V W X
  52, 89,			  // Y Z
  // Numbers
  23, 24, 26, 27, 28, 30, 31, 32, 34, 35, // 1 2 3 4 5 6 7 8 9 0
  // Other printing chars
  36, 37, 58, 59, 60, 79, 80, 98, // -_ += [{ ]} \| ;: '" ,<
  100, 101, 22,			  // .> /? `~
  // Nonprinting keys
  0, 44, 66,			// Esc Tab Capslock
  1, 2, 3, 4, 5, 6,		// F1 F2 F3 F4 F5 F6
  8, 9, 10, 11, 12, 13,		// F7 F8 F9 F10 F11 F12
  81, 95, 			// REnter MEnter
  88, 118, 102,			// LShift MShift RShift
  110, 116, 124,		// LControl MControl RControl
  112, 120,			// LAlt RAlt
  73, 38,			// MBackspace RBackspace
  115, 119,			// LSpace RSpace
  111,				// Windows
  61, 83,			// PageUp PageDown
  125, 127, 104, 126,		// Left Right Up Down
  16, 14,			// PrtScrn Delete
  51,				// XBows
  // Numpad
  0xff, 0xff, 0xff, 0xff, 	// Numlock NPSlash NPStar NPEnter
  0xff, 0xff, 0xff, 0xff,	// NP1 NP2 NP3 NP4
  0xff, 0xff, 0xff, 0xff,	// NP5 NP6 NP7 NP8
  0xff, 0xff, 0xff, 0xff, 0xff,	// NP9 NP0 NP. NP- NP+
  // Media
  0xff, 0xff, 0xff, 0xff,	// Play Pause Stop Last
  0xff, 0xff, 0xff, 0xff,	// Next VolUp VolDown Mute
  // Mouse
  0xff, 0xff, 0xff, 0xff, 0xff,	// LClick MClick RClick Back Forward
  // Sys/net
  0xff, 0xff, 0xff, 0xff,	// NetBack NetFwd NetRefresh NetCollection
  0xff, 0xff, 0xff, 0xff,	// NetHome NetEmail NetComp NetCalc
  0xff, 0xff, 0xff,		// NetCopy NetPaste NetPrtScrn
  // Not on xbows kbd
  0xff, 0xff, 0xff,		// Home End Insert
  // Function key
  123				// Fn

};


vector<drv_light_frame> make_calc() {
  // Start with a green calculator light map.
  vector<drv_light_frame> framelist;

  // To write the program, user needs to be able to specify that K8 is green.
  // This means that K8->34 finds rgb[9] and sets it to 00ff0064.
  drv_light_frame frame;
  uint32_t green = 0x00ff00;
  frame.setkey(K_8, green);
  frame.setkey(K_9, green);
  frame.setkey(K_U, green);
  frame.setkey(K_I, green);
  frame.setkey(K_O, green);
  frame.setkey(K_P, green);
  frame.setkey(K_J, green);
  frame.setkey(K_K, green);
  frame.setkey(K_L, green);
  frame.setkey(K_Semi, green);
  frame.setkey(K_M, green);
  frame.setkey(K_Comma, green);
  frame.setkey(K_Period, green);
  frame.setkey(K_Slash, green);
  frame.setkey(K_RSpc, green);
  frame.setkey(K_RAlt, green);

  framelist.push_back(frame);

  return framelist;
}

// JQ just activate each key one frame at a time
vector<drv_light_frame> make_trail() {
  // Start with a green calculator light map.
  vector<drv_light_frame> framelist;

  // To write the program, user needs to be able to specify that K8 is green.
  // This means that K8->34 finds rgb[9] and sets it to 00ff0064.
  drv_light_frame frame;
  uint32_t green = 0x00ff00;

  frame.setkey(K_Esc, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F1, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F2, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F3, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F4, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F5, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F6, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F7, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F8, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F9, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F10, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_F12, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_Del, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_PrtScrn, green);
  framelist.push_back(frame);
  frame.clear();

  frame.setkey(K_RBksp, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_Equal, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_Dash, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_0, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_9, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_8, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_7, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_6, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_5, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_4, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_3, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_2, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_1, green);
  framelist.push_back(frame);
  frame.clear();
  frame.setkey(K_Backquote, green);
  framelist.push_back(frame);

  vector<drv_light_frame> framelist2(framelist);
  framelist2.insert(framelist2.end(), framelist.begin(), framelist.end());
  
  return framelist2;
}


//////////////////////

// Set up the default keymap
void drv_keymap::clear() {
  // Iterate through table and assign values to kmap

  // I need to assign LCtrl position 3 with value 01000002, etc.
  // Unused/unknown slots get 0xffffffff.

  // How do I track the position mapping?  drv_keymap_assign.

  // There is a keyid map that is shared between driver and custom.
  // That gives me the value to write into the slot.

  // First fill with 0xff for unused/unknown slots
  memset(keys, 0xff, MAX_KEYMAP * 4);

  // Now set specific key slots
  for (int i=0; i < MAX_KEYCODE; i++) {
    uint16_t id = keyid[i];	// 2 of the 4 bytes we need
    int pos = drv_keymap_assign[i];

    if (id == 0xffff) continue;
    if (pos == 0xff) continue;

    // Each value looks like id 00 02
    keys[pos] = 0x02000000 | (uint32_t)htobe16(id);
  }

}


// Map from keycode to position in drv_keymap
int drv_keymap_assign[MAX_KEYCODE] = {
  // Position 0 is empty
  0xff,				  // K_NONE
  // Letters
  10, 11, 12, 13,			  // A B C D packet 1
  14, 15, 16, 17, 18, 19, 20, 21,	  // E F G H I J K L packet 2
  22, 23, 24, 25, 26, 27,		  // M N O P Q R packet 2
  28, 29, 30, 31, 32, 33, 34, 35,	  // S T U V W X Y Z packet 3
  // Numbers
  36, 37, 38, 39, 40, 41,		  // 1 2 3 4 5 6 packet 3
  42, 43, 44, 45,			  // 7 8 9 0 packet 4
  // Other printing chars
  51, 52, 53, 54, 55,		// -_ += [{ ]} \| packet 4
  56, 57, 59,			// ;: '" ,<
  60, 61, 58,			// .> /? `~
  // Nonprinting keys
  47, 49, 62,			// Esc Tab Capslock
  63, 64, 65, 66, 67, 68, 69,	// F1 F2 F3 F4 F5 F6 F7
  70, 71, 72, 73, 74,		// F8 F9 F10 F11 F12
  46, 113, 			// REnter MEnter
  3, 115, 7,			// LShift MShift RShift
  2, 114, 6,			// LControl MControl RControl
  4, 8,				// LAlt RAlt
  112, 48,			// MBackspace RBackspace
  50, 116,			// LSpace RSpace
  5,				// Windows
  80, 83,			// PageUp PageDown
  85, 84, 87, 86,		// Left Right Up Down
  75, 81,			// PrtScrn Delete
  0xff,				// XBows
  // Numpad
  0xff, 0xff, 0xff, 0xff, 	// Numlock NPSlash NPStar NPEnter
  0xff, 0xff, 0xff, 0xff,	// NP1 NP2 NP3 NP4
  0xff, 0xff, 0xff, 0xff,	// NP5 NP6 NP7 NP8
  0xff, 0xff, 0xff, 0xff, 0xff,	// NP9 NP0 NP. NP- NP+
  // Media
  0xff, 0xff, 0xff, 0xff,	// Play Pause Stop Last
  0xff, 0xff, 0xff, 0xff,	// Next VolUp VolDown Mute
  // Mouse
  0xff, 0xff, 0xff, 0xff, 0xff,	// LClick MClick RClick Back Forward
  // Sys/net
  0xff, 0xff, 0xff, 0xff,	// NetBack NetFwd NetRefresh NetCollection
  0xff, 0xff, 0xff, 0xff,	// NetHome NetEmail NetComp NetCalc
  0xff, 0xff, 0xff,		// NetCopy NetPaste NetPrtScrn
  // Not on xbows kbd
  0xff, 0xff, 0xff,		// Home End Insert
  // Function key
  0xff				// Fn
};

void drv_keymap::assign(keycodes key, keycodes emits) {
  keys[drv_keymap_assign[K_Z]] =
    htobe16(keyid[K_Q]) | 0x02000000;
}


vector<packet> keymap_program(drv_keymap& kmap) {
  // For each key, we want an RGB value.  Convert to a sequence of packets.
  // Sequence must start with 0x0b05
  // Sequence must next have 0x0109
  init_driver_mode();

  // Driver mode light program is 10 packets.
  vector<packet> program;

  // Start from standard keymap
  // Reassign keys that are different


  // Convert to packet program

  // Get the keyboard's attention.
  program.assign(drv_attn.begin(), drv_attn.end());

  // XXXXX DEBUG WRITING KEYMAP TO PACKETS

  // Light program.  14 key rgb per packet
  int remaining = kmap.size();
  for (int i=0; i < kmap.size(); i+=14, remaining-=14) {
    packet pkt(0x16, 0x01);

    // Number of bytes already added to the program
    pkt.progcount = htole16(i * 4);

    // Store bytes of packet data in byte 5
    unsigned char pktbytes = min(remaining * 4, 56);
    pkt.datasize = pktbytes;

    // Copy the packet data
    for (int j=0; j < min(remaining, 14); j++) {
      uint32_t val = htole32(kmap.keys[i+j]);
      memcpy(pkt.data+j*4, &val, 4);
    }
    program.push_back(pkt);
    // cout << "push packet " << pkt.to_string();
  }

  // Default flashlight keymap packets
  program.push_back(packet(0x16, 0x03));
  memset(program.back().data, 0xff, 56);
  program.back().datasize = 56;
  program.back().progcount = 0;

  program.push_back(packet(0x16, 0x03));
  memset(program.back().data, 0xff, 56);
  program.back().datasize = 56;
  program.back().progcount = 0x38;

  program.push_back(packet(0x16, 0x03));
  memset(program.back().data, 0xff, 8);
  program.back().datasize = 8;
  program.back().progcount = 0x70;


  // Terminator packet
  program.push_back(packet(0x17, 0x01));
  memset(program.back().data, 0, 56);
  program.back().data[0] = 0xf0;
  program.back().data[1] = 0x01;
  program.back().data[2] = 0x12;
  program.back().data[3] = 0x03;
  program.back().data[8] = 0x0f;

  // XXX
  // ADD writing macro if specified
  // ADD writing flashlight data if we figure that out 0x18 command



  // assert(program.size() == 11);

  // Compute crc for each packet
  for (auto& pkt: program)
    pkt.compute_crc();

  return program;
}
