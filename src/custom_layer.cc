// Control custom layer

// We need to convert a keyboard program to a sequence of packets.  This code
// is similar but not identical to the driver layer program.

#include <endian.h>
extern "C" {
#include <checksum.h>
}
#include <cassert>
#include <cmath>
#include <vector>

#include <iostream>

#include "xbows.hh"
#include "custom_layer.hh"
// For keymap_assign[]
#include "layer.hh"
#include "keymap.hh"

using namespace std;

uint16_t& addr_to_16(unsigned char* addr) { return *(uint16_t*)addr; }
uint32_t& addr_to_32(unsigned char* addr) { return *(uint32_t*)addr; }


// Pack bytes into program, adding more packets as needed.  This function also
// fills in packet byte counts as needed.
vector<packet>& pack_data(vector<packet>& program,
			  unsigned char* data, int count, bool datasize2=false) {
  // Assumes there is a packet at the end of program to look at
  assert(program.size());
  while (count) {
    // Add new packet if needed.
    if ((datasize2 && program.back().nil == 56)
	||
	(!datasize2 && program.back().datasize == 56)) {
      packet& pkt = program.back();
      program.push_back(packet(pkt.cmd, pkt.sub));

      // Update program bytes preceding this packet
      uint16_t progcount = le16toh(pkt.progcount) + 56;
      program.back().progcount = progcount;
    }
    
    // Try to fill the current packet
    packet& pkt = program.back();

    // Pack up to 56 bytes of data into packet
    int datasize = datasize2 ? pkt.nil : pkt.datasize;
    int pcount = min(count, 56-datasize);
    memcpy(pkt.data + datasize, data, pcount);
    if (datasize2)
      pkt.nil += pcount;
    else
      pkt.datasize += pcount;
    
    count -= pcount;
    data += pcount;
  }
  return program;
}


// For custom layer commands, the subcommand indicates the layer 0x01, 0x02,
// 0x03 are custom layers 1-3.
//
// Handles rebinding and disabling keys.
//
vector<packet> custom_keymap_program(int layer, program& prog) {
  if (layer < CUSTOM1 || layer > CUSTOM3)
    throw runtime_error("Bad layer");

  // Make a copy so macros can override the basic keymap data.
  keymap kmap = prog.kmap;

  vector<packet> program;

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


vector<packet> custom_macro_program(int layer, program& prog) {
  if (layer < CUSTOM1 || layer > CUSTOM3)
    throw runtime_error("Bad layer");

  vector<uint32_t> macro_codes;
  // macro header + crc in bytes 2,3
  uint32_t code;
  for (auto& macro : prog.macros) {
    unsigned start = macro_codes.size();
    // Add macro header and space for crc code.
    macro_codes.push_back(0x000055aa);
    // Set macro length, mode, and loop count.
    if (macro.steps.size() > 255 || macro.id > 255 || macro.count > 255)
      throw runtime_error("Macro field is out of range > 255\n");
    code = htole32(macro.steps.size());
    code |= macro.id << 8;
    int count = (macro.count & 0xff) << 24;
    int mode = (macro.play_mode & 0xff) << 16;
    code |= htole32(count|mode);
    macro_codes.push_back(code);
    // Add each macro step
    for (auto& step : macro.steps) {
      if (step.duration) {
	// Add a delay step
	code = htole32(step.duration);
	code |= htole32(0x03000000);
      }
      else {
	// Add a key press/release step
	code = keyid[step.key];
	code |= htole32(0x01000000);
	code |= step.down
	  ? htole32(0x00010000)
	  : htole32(0x00020000);
      }
      macro_codes.push_back(code);
    }

    // Compute crc on macro sequence before converting to packets CRC goes in
    // upper 2 bytes of the 0xaa55 int.  First 2 ints are ignored for
    // computing the CRC.
    uint16_t crc = crc_ccitt_ffff((unsigned char*)(macro_codes.data()+2),
				  sizeof(uint32_t) * (macro_codes.size()-2));
    macro_codes[start] |= htole16(crc) << 16;

    // Pad macro to 128 ints
    unsigned end = macro_codes.size();
    unsigned pad = 128 - (end-start);
    macro_codes.insert(macro_codes.end(), pad, 0);
  }
  

  // Copy macro sequence data into packets
  // xxx make sure this gets injected after 0x210x04, before 0x210x05.
  vector<packet> packets;
  packet pkt(0x25, layer);
  packets.push_back(pkt);
  pack_data(packets, (unsigned char*)macro_codes.data(),
	    macro_codes.size() * sizeof(uint32_t), true);
  
  return packets;
}


// XXX for now we have a default inactive flashlight sequence
// It seems there must be 3 packets of flashlight even when nothing is
// programmed.  XXX Is this true?
vector<packet> custom_flashlight_program(int layer, program& prog) {
  if (layer < CUSTOM1 || layer > CUSTOM3)
    throw runtime_error("Bad layer");
  
  vector<packet> program;

  packet pkt1(0x26, layer);
  memcpy(pkt1.data, prog.flashlight_keys, 56);
  pkt1.nil = 0x38;		// Datasize is reversed here
  program.push_back(pkt1);

  packet pkt2(0x26, layer);
  memcpy(pkt2.data, prog.flashlight_keys + 56, 56);
  pkt2.progcount = htole16(0x38);
  pkt2.nil = 0x38;		// Datasize is reversed here
  program.push_back(pkt2);

  packet pkt3(0x26, layer);
  memcpy(pkt3.data, prog.flashlight_keys + 112, 8);
  pkt3.progcount = htole16(0x70);
  pkt3.nil = 0x8;		// Datasize is reversed here
  program.push_back(pkt3);
  
  return program;
}

// Constructor
animation_frame::animation_frame() {
  header = htole32(0x00160003);
  clear();
}


// Map from keycode to bit position in animation frame.  Each key gets
// assigned to a byte and a bit within the byte.  The least significant nibble
// is the bit position.  The most significant nibbles are the byte position.
// int is overkill but we need more than 8 bits to represent the byte position
// because the bitmap is 22 bytes long.
//
// 0xff is unassigned.
int animation_assign[MAX_KEYCODE] = {
  // Position 0 is empty
  0xff,				  // K_NONE
  // Letters
  0x83, 0xb6, 0xb4, 0x86,			  // A B C D 
  0x60, 0x87, 0x90, 0x92, 0x66, 0x93, 0x94, 0x96, // E F G H I J K L
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
  0xa1, 0xb7,		      // REnter MEnter
  0xb0, 0xe6, 0xc6,	      // LShift MShift RShift
  0xd6, 0xe4, 0xf4,	      // LControl MControl RControl
  0xe0, 0xf0,		      // LAlt RAlt
  0x91, 0x46,		      // MBackspace RBackspace
  0xe3, 0xe7,		      // LSpace RSpace
  0xd7,			      // Windows
  0x75, 0xa3,		      // PageUp PageDown
  0xf5, 0xf7, 0xd0, 0xf6,     // Left Right Up Down
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

// Enable the bit associated with key in keymap.  keymap is a 22 char bitmap.
// Same for both animation and light frame keymaps.
void enable_key(uint8_t* keymap, keycodes key) {
  int code = animation_assign[key];
  int byte = code >> 4;                // byte position is everything but lowest 4 bits
  int bit = code & 0xf;                // Low 4 bits is the bit to enable
  int mask = 1 << bit;
  keymap[byte] |= mask;
}


// Enable the bit associated with key in keymap.  keymap is a 22 char bitmap.
void animation_frame::enable(keycodes key) { enable_key(keymap, key); }

// Enable all keys for frame
void animation_frame::all() { memset(keymap, 0xff, 22); }

// Clear the bitmap
void animation_frame::clear() { memset(keymap, 0, 22); }

// Enable the bit associated with key in keymap.  keymap is a 22 char bitmap.
void pattern_frame::enable(keycodes key) { enable_key(keymap, key); }

// Clear the bitmap
void pattern_frame::clear() { memset(keymap, 0, 22); }

// Enable all keys for frame
void pattern_frame::all() { memset(keymap, 0xff, 22); }


// Store a single custom light program into packets
void custom_light_program(vector<packet>& program,
			  custom_light_prog& frames) {
  // Add animation frames
  for (auto f: frames.aframes)
    pack_data(program, f.data, 26);

  // Add light frames
  for (auto f: frames.lframes)
    pack_data(program, f.data, 32);
}


// Multiple lighting programs can actually be set up.  First is for the
// regular custom light.  The others are flashlight programs.  They all get
// assembled into packets here.
vector<packet> custom_light_programs(int layer, program& prog) {
  if (layer < CUSTOM1 || layer > CUSTOM3)
    throw runtime_error("Bad layer");

  // Initialize packet sequence with 1 empty packet.
  vector<packet> packets(1, packet(0x27, (unsigned char)layer));

  // Construct frame info bytes
  int infosize = prog.flashlights.size() + 1; // +1 for custom_lights
  infosize *= 16;			 // bytes consumed by frame info
  unsigned char info[infosize];
  uint32_t* info_p = (uint32_t*)info;

  // Frame info for custom lights
  int anim_ct = prog.custom_lights.aframes.size();
  int lite_ct = prog.custom_lights.lframes.size();

  unsigned int framestart = 0x0200; // Start of anim frames for custom light.
  *info_p++ = htole32(framestart);
  *info_p++ = htole32(anim_ct);
  framestart += anim_ct * 0x1a;	// start of light frames
  *info_p++ = htole32(framestart);
  *info_p++ = htole32(lite_ct);
  framestart += lite_ct * 0x20;	// start of light frames

  // Frame info for flashlights
  for (unsigned i=0; i < prog.flashlights.size(); i++) {
    // Frame info for custom lights
    int anim_ct = prog.flashlights[i].aframes.size();
    int lite_ct = prog.flashlights[i].lframes.size();

    *info_p++ = htole32(framestart);
    *info_p++ = htole32(anim_ct);
    framestart += anim_ct * 0x1a;	// start of light frames
    *info_p++ = htole32(framestart);
    *info_p++ = htole32(lite_ct);
    framestart += lite_ct * 0x20;	// start of light frames
  }

  // Pack frame info
  pack_data(packets, info, infosize);

  
  // Fill remaining space before lighting programs
  if (infosize > 0x200)
    throw runtime_error("No more than 128 lighting programs can be specified");
  int fillbytes = 0x200 - infosize;

  // Fill in 124 ints of 0xffffffff into last packet, 8 full packets, and
  // start of another packet.
  unsigned char ffs[fillbytes];
  memset(ffs, 0xff, fillbytes);
  pack_data(packets, ffs, fillbytes);
  

  // Prepare lighting programs

  // First store custom_lights
  custom_light_program(packets, prog.custom_lights);
  
  
  // Now store flashlights
  for (auto flash : prog.flashlights)
    custom_light_program(packets, flash);
  
  return packets; 
}


custom_light_prog no_lights() {
  custom_light_prog prog;
  prog.aframes.push_back(animation_frame());
  prog.lframes.push_back(pattern_frame());
  return prog;
}


// We have to assemble a complete program to send
vector<packet> custom_program(char layer, program& prog) {

  // Make sure things are consistent

  // If there is a flashlight program, there must also be a base light program
  // or else first flashlight will become the base light program.
  if (prog.flashlights.size() && prog.custom_lights.aframes.empty())
    prog.custom_lights = no_lights();
  
  vector<packet> program;

  // Get the keyboard's attention.
  program.assign(drv_attn.begin(), drv_attn.end());

  
  // Set up the keymap program
  packet keymap_intro(0x21, layer);
  keymap_intro.bytes[2] = 0x01;
  program.push_back(keymap_intro);
  
  vector<packet> keyprog = custom_keymap_program(layer, prog);
  program.insert(program.end(), keyprog.begin(), keyprog.end());


  // Set up macro program
  packet macro_intro(0x21, layer);
  macro_intro.bytes[2] = 0x04;
  program.push_back(macro_intro);
  
  vector<packet> macroprog = custom_macro_program(layer, prog);
  program.insert(program.end(), macroprog.begin(), macroprog.end());


  // Set up flashlight program
  packet flash_intro(0x21, layer);
  flash_intro.bytes[2] = 0x05;
  program.push_back(flash_intro);
  
  vector<packet> flashprog = custom_flashlight_program(layer, prog);
  program.insert(program.end(), flashprog.begin(), flashprog.end());


  
  // Set up light program
  packet light_intro(0x21, layer);
  light_intro.bytes[2] = 0x06;
  program.push_back(light_intro);


  // Construct the lighting program
  vector<packet> lightprograms = custom_light_programs(layer, prog);
  program.insert(program.end(), lightprograms.begin(), lightprograms.end());


  packet terminator(0x0b, layer);
  program.push_back(terminator);


  // Compute crc for each packet
  for (auto& pkt: program)
    pkt.compute_crc();

  return program;
}


