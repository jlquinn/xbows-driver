// Control custom layer

// We need to convert a keyboard program to a sequence of packets.  This code
// is similar but not identical to the driver layer program.

#include <cmath>
#include <vector>

#include "custom_layer.hh"
#include "keymap.hh"

using namespace std;

uint16_t& addr_to_16(unsigned char* addr) { return *(uint16_t*)addr; }
uint32_t& addr_to_32(unsigned char* addr) { return *(uint32_t*)addr; }


// Custom program

// animation keymap is a 22 byte bitmap representing keys
// struct keymap {
//   unsigned char[22];
// };


// struct light_frame {
//   uint16_t header = 0x0020;
//   keymap keys;
//   uint32_t rgb0;
//   uint16_t inv_dur = 0;		// rgb and breathing set this
//   uint16_t terminator = 0x7900;	// unchanged for monochrome and rgb

//   // RGB cycle duration
//   void set_rgb_cycle(int d) {
//     int inv = floor(360/d);
//     inv_dur = (short)inv;
//   }

//   // Breathing duration
//   void set_breathe_cycle(int dur, uint16_t stop) {
//     int inv = floor(100/d);
//     inv_dur = (short)inv;
//     terminator = stop * 0xffff;
//   }
// };


// // This is essentially a mask over whatever lighting is active.
// struct animation_frame {
//   uint32_t header = 0x03001600;
//   keymap keys;
// };


// // Lighting program is a combination of light frames and animation frames.
// struct light_program {
//   uint32_t start = 0x00020000;
//   uint32_t anim_frames;
//   uint32_t anim_duration;
//   uint32_t light_frames;

//   keymap light_keymap;

// };

// struct macro_program {

// };

// struct keymap_program {
// };

// struct flashlight_program {

// };

// Map from keycode to position in drv_keymap
int cus_keymap_assign[MAX_KEYCODE] = {
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

void cus_keymap::assign(keycodes key, keycodes emits) {
  keys[cus_keymap_assign[K_Z]] =
    htobe16(keyid[K_Q]) | 0x02000000;
}


// Set up the default keymap.  Gets called when a cus_keymap is created.
// This code is identical to the default driver keymap.
void cus_keymap::clear() {
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
    int pos = cus_keymap_assign[i];

    if (id == 0xffff) continue;
    if (pos == 0xff) continue;

    // Each value looks like id 00 02
    keys[pos] = 0x02000000 | (uint32_t)htobe16(id);
  }

}


// For custom layer commands, the subcommand indicates the layer 0x01, 0x02,
// 0x03 are custom layers 1-3.
//
// Handles rebinding and disabling keys.
//
vector<packet> custom_keymap_program(int layer// , cus_keymap& kmap
				     ) {
  if (layer < 1 || layer > 3)
    throw runtime_error("Bad layer");
  
  vector<packet> program;

// XXX Default keymap for now
  cus_keymap kmap;

  // Light program.  14 key rgb per packet
  int remaining = kmap.size();
  for (int i=0; i < kmap.size(); i+=14, remaining-=14) {
    packet pkt(0x22, layer);	// This is different from driver_keymap prog

    // Number of bytes already added to the program
    pkt.progcount = htole16(i * 4);

    // Store bytes of packet data in byte 5
    unsigned char pktbytes = min(remaining * 4, 56);
    pkt.nil = pktbytes;		// XXX NEED BETTER HANDLING OF THIS

    // Copy the packet data
    for (int j=0; j < min(remaining, 14); j++) {
      uint32_t val = htole32(kmap.keys[i+j]);
      memcpy(pkt.data+j*4, &val, 4);
    }
    program.push_back(pkt);
    // cout << "push packet " << pkt.to_string();
  }
  
  return program;
}

// XXX empty macros for now
vector<packet> custom_macro_program(int layer) {
  if (layer < 1 || layer > 3)
    throw runtime_error("Bad layer");

  return vector<packet>();
}


// XXX for now we have a default inactive flashlight sequence
// It seems there must be 3 packets of flashlight even when nothing is
// programmed.  XXX Is this true?
vector<packet> custom_flashlight_program(int layer) {
  if (layer < 1 || layer > 3)
    throw runtime_error("Bad layer");
  
  vector<packet> program;

  packet pkt1(0x26, layer);
  memset(pkt1.data, 0xff, 56);
  pkt1.nil = 0x38;		// Datasize is reversed here
  program.push_back(pkt1);

  packet pkt2(0x26, layer);
  memset(pkt2.data, 0xff, 56);
  pkt2.progcount = htole16(0x38);
  pkt2.nil = 0x38;		// Datasize is reversed here
  program.push_back(pkt2);

  packet pkt3(0x26, layer);
  memset(pkt3.data, 0xff, 8);
  pkt3.progcount = htole16(0x70);
  pkt3.nil = 0x8;		// Datasize is reversed here
  program.push_back(pkt3);
  
  return program;
}


// Map from keycode to bit position in animation frame.  Each key gets
// assigned to a byte and a bit within the byte.  For now I'm cheating.  The
// first nibble is the byte 0-15, and the second nibble is the bit position.
//
// 0xff is unassigned.
int cus_anim_assign[MAX_KEYCODE] = {
  // Position 0 is empty
  0xff,				  // K_NONE
  // Letters
  0x83, 0xb6, 0xb4, 0x86,			  // A B C D 
  0x60, 15, 16, 17, 0x66, 19, 0x94, 0x96,	  // E F G H I J K L
  0xc1, 0xc0, 0x70, 0x71, 0x55, 0x61,		  // M N O P Q R
  0x84, 0x62, 0x65, 0xb5, 0x56, 0xb2, 0x64, 0xb1,	  // S T U V W X Y Z
  // Numbers
  0x27, 0x30, 0x32, 0x33, 0x34, 0x36,	    // 1 2 3 4 5 6 
  0x37, 0x40, 0x42, 0x43,		    // 7 8 9 0 
  // Other printing chars
  0x44, 0x45, 0x72, 0x73, 0x74,		    // -_ += [{ ]} \|
  0x97, 0xa0, 0xc2,			    // ;: '" ,<
  0xc4, 0xc5, 0x26,			    // .> /? `~
  // Nonprinting keys
  0x00, 0x54, 0x82,			    // Esc Tab Capslock
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x10, // F1 F2 F3 F4 F5 F6 F7
  0x11, 0x12, 0x13, 0x14, 0x15,		    // F8 F9 F10 F11 F12
  0xa1, 0xb7,		      // REnter MEnter  XXXX spec is unsure of these 2
  0xb0, 0xe6, 0xc6,	      // LShift MShift RShift
  0xd6, 0xe4, 0xf4,	      // LControl MControl RControl
  0xe0, 0xf0,		      // LAlt RAlt
  112, 0x46,		      // MBackspace RBackspace
  0xe3, 0xe7,		      // LSpace RSpace
  0xd7,			      // Windows
  0x75, 0xa3,		      // PageUp PageDown
  0xf6, 0xf7, 0xd0, 0xf6,     // Left Right Up Down
  0x20, 0x16,		      // PrtScrn Delete
  0x63,			      // XBows
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
  0xf3				// Fn
};

// TODO code to pack animation and light frames into packets



// Light program is mode complex than driver mode.  We have animation frames
// and light frames.

vector<packet> custom_light_program(int layer// ,
				    // const vector<cus_light_frame>& framelist
				    ) {
  if (layer < 1 || layer > 3)
    throw runtime_error("Bad layer");

  // unsigned char layercode = (unsigned char)layer;

  // XXXX Driver mode light program is 10 packets.
  vector<packet> program;

  // Starting with alternating Esc and F1 in green.
  // Mono green light, 2 animation frames
  unsigned short bytes = 0;
  packet pkt(0x27, layer);
  pkt.progcount = bytes;
  pkt.datasize = 0x38;

  // Start of lighting program.  I think.
  addr_to_32(pkt.data) = htole32(0x0200);
  addr_to_32(pkt.data+4) = htole32(0x02);

  // 2 animation frames
  uint32_t anim_dur = 2;
  anim_dur *= 0x1a;
  anim_dur += 0x0200;		// I don't know why we add 0x0200
  addr_to_32(pkt.data+8) = htole32(anim_dur);
  addr_to_32(pkt.data+12) = htole32(1); // 1 lighting frame
  memset(pkt.data+12, 0xff, 44);       // Fill remainder with ff

  program.push_back(pkt);

  // Add in 0xff packets.  We end up with 120 ints of 0xffffffff including
  // previous and following packet.
  for (int i=0; i < 8; i++) {
    packet pkt(0x27, layer);
    bytes += 0x38;
    pkt.progcount = htole16(bytes);
    pkt.datasize = 0x38;
    memset(pkt.data, 0xff, 56);
    program.push_back(pkt);
  }

  // Animation frames
  // Anim frame 1
  bytes += 0x38;
  packet pkt2(0x27, layer);
  pkt2.datasize = 0x38;
  pkt2.progcount = htole16(bytes); // XXX hardwired
  // Last two 0xfffffffff spots
  addr_to_32(pkt2.data) = 0xffffffff;
  addr_to_32(pkt2.data + 4) = 0xffffffff;

  // First animation frame
  addr_to_32(pkt2.data + 8) = 0x00160003; // animation frame header
  pkt2.data[12] = 0x02;			 // Unmask Esc

  // Second animation frame;
  addr_to_32(pkt2.data + 34) = 0x00160003; // animation frame header
  pkt2.data[38] = 0x01;			  // Unmask F1

  program.push_back(pkt2);

  // 2nd animation frame continues into next packet
  packet pkt3(0x27, layer);
  pkt3.datasize = 36;		// last 4 bytes anim frame 2, 32 bytes light frame
  bytes += 0x38;
  pkt3.progcount = htole16(bytes);

  // Start of light frame
  addr_to_16(pkt3.data+4) = htole16(0x2000);
  pkt3.data[6] = 0x03;				  // Light up Esc and F1 
  addr_to_32(pkt3.data+28) = htole32(0x000000ff); // RGB red
  addr_to_32(pkt3.data+32) = htole32(0x00790000); // frame terminator
  
  program.push_back(pkt3);

  
  return program;
}


// We have to assemble a complete program to send
vector<packet> custom_program(char layer) {
  init_xbows();

  vector<packet> program;

  // Get the keyboard's attention.
  program.assign(drv_attn.begin(), drv_attn.end());

  
  // Set up the keymap program
  packet keymap_intro(0x21, layer);
  keymap_intro.bytes[2] = 0x01;
  program.push_back(keymap_intro);
  
  vector<packet> keyprog = custom_keymap_program(layer);
  program.insert(program.end(), keyprog.begin(), keyprog.end());


  // Set up macro program
  packet macro_intro(0x21, layer);
  macro_intro.bytes[2] = 0x04;
  program.push_back(macro_intro);
  
  vector<packet> macroprog = custom_macro_program(layer);
  program.insert(program.end(), macroprog.begin(), macroprog.end());


  // Set up flashlight program
  packet flash_intro(0x21, layer);
  flash_intro.bytes[2] = 0x05;
  program.push_back(flash_intro);
  
  vector<packet> flashprog = custom_flashlight_program(layer);
  program.insert(program.end(), flashprog.begin(), flashprog.end());


  
  // Set up light program
  packet light_intro(0x21, layer);
  light_intro.bytes[2] = 0x06;
  program.push_back(light_intro);
  
  vector<packet> lightprog = custom_light_program(layer);
  program.insert(program.end(), lightprog.begin(), lightprog.end());


  packet terminator(0x0b, layer);
  program.push_back(terminator);


  // Compute crc for each packet
  for (auto& pkt: program)
    pkt.compute_crc();

  return program;
}

