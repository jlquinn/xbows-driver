// Control Xbows driver layer.
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
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


// Generate packets for a driver mode light program.  To do this, I need to
// assign each key an RGB value.  The return needs to contain the full packet
// sequence, as well as expected return codes from the keyboard to make sure
// nothing is going wrong.
vector<packet> light_program() {
  // For each key, we want an RGB value.  Convert to a sequence of packets.
  // Sequence must start with 0x0b05
  // Sequence must next have 0x0109

  // Driver mode light program is 10 packets.
  vector<packet> program;

  // Start with a green calculator light map.

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

  // Convert to packet program

  // Initial program packets.  NO, this is for keymap, not lighting.
  // program.push_back(packet(0x0b, 0x05));
  // program.push_back(packet(0x01, 0x09));

  // Light program.  14 key rgb per packet
  unsigned short bytes = 0;
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

  assert(program.size() == 11);

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
