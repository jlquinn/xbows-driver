#ifndef PACKET_HH
#define PACKET_HH

#include <cstring>
#include <array>

// Packet structure
// byte 0:	command
// byte 1:	subcommand
// byte 2,3:	byte count.  Number of data bytes already sent
// byte 4:	NOT SURE, sometimes valid data bytes in the packet
// byte 5:	number of valid data bytes in this packet
// byte 6,7:	packet crc.  crc-16/ccitt-false with 6,7 set to 0
// byte 8-63:	data bytes
typedef std::array<unsigned char, 64> pktarray;
struct packet {
  union {
    unsigned char bytes[64];	// This is a HID packet sent to the keyboard
    struct {
      unsigned char cmd, sub;
      unsigned short progcount;
      unsigned char nil, datasize;
      unsigned short crc;
      unsigned char data[56];
    };
  };

  packet() {}
  packet(unsigned char cmd, unsigned char subcmd) : cmd(cmd), sub(subcmd) {
    memset(bytes+2, 0, 62);
  }
  packet(const std::array<unsigned char, 64> a) {
    memcpy(bytes, a.data(), a.size());
  }
  
  void compute_crc();

};

#endif