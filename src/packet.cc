// Basic packet handling for xbows programming interface

#include <endian.h>
extern "C" {
#include <checksum.h>
}

#include <iomanip>
#include <sstream>
#include <string>

#include "packet.hh"

using namespace std;

// Packet structure
// byte 0:	command
// byte 1:	subcommand
// byte 2,3:	byte count.  Number of data bytes already sent
// byte 4:	NOT SURE, sometimes valid data bytes in the packet
// byte 5:	number of valid data bytes in this packet
// byte 6,7:	packet crc.  crc-16/ccitt-false with 6,7 set to 0
// byte 8-63:	data bytes

void packet::compute_crc() {
  // Compute the packet crc
  bytes[6] = bytes[7] = 0;
  uint16_t crc = crc_ccitt_ffff(bytes, 64);

  // Assign little-endian crc to bytes 6 and 7
  crc = htole16(crc);
  *(uint16_t*)(bytes+6) = crc;
}

string packet::to_string() const {
  ostringstream os;
  os << "Packet:\n";
  for (int i=0; i < 64; i++) {
    os << " " << hex << setw(2) << setfill('0') << (unsigned short)bytes[i];
    //printf(" %02x", data[i]);
    if ((i+1) % 16 == 0) os << "\n";
  }
  return os.str();
}


enum commands {
  IDLE=0x0c,
  ATTN=0x0b,			// start of driver mode programming, end of
				// custom layer programming.
  DRVSTART=0x01,
  DRVLIGHT=0x1a,		// Driver mode LED control
  DRVKEY=0x16,			// Driver mode keymap setup
  DRVEND=0x17,
  CUSTSEP=0x21,
  CUSTKEYMAP=0x22,
  CUSTMACRO=0x25,
  CUSTFLASH=0x26,
  CUSTLIGHT=0x27,
};



vector<packet> drv_attn;
void init_packet() {
  if (drv_attn.empty()) {
    // Initial program packets.  This gets the keyboard's attention.
    drv_attn.push_back(packet(0x0b, 0x03)); // 03 is the layer selection
    drv_attn.push_back(packet(0x01, 0x09));

    // Compute crc for each packet
    for (auto& pkt: drv_attn)
      pkt.compute_crc();
  }
}
