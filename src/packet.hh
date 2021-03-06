#ifndef PACKET_HH
#define PACKET_HH

#include <cstring>
#include <array>
#include <string>
#include <vector>

// Packet structure
// byte 0:	command
// byte 1:	subcommand
// byte 2,3:	byte count.  Number of data bytes already sent
// byte 4:	NOT SURE, sometimes valid data bytes in the packet
// byte 5:	number of valid data bytes in this packet
// byte 6,7:	packet crc.  crc-16/ccitt-false with 6,7 set to 0
// byte 8-63:	data bytes
// data is little-endian byte order
typedef std::array<unsigned char, 64> pktarray;
struct packet {
  int delay;			// delay in ms before sending this packet
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

  packet() : delay(0) {}
  packet(unsigned char cmd, unsigned char subcmd)
    : delay(0), cmd(cmd), sub(subcmd)
  {
    memset(bytes+2, 0, 62);
  }
  packet(const std::array<unsigned char, 64> a) : delay(0) {
    memcpy(bytes, a.data(), a.size());
  }
  
  void compute_crc();

  std::string to_string() const;
};


// The program calls init_driver_mode once.  Then it can create and send light
// programs as needed.

// This is the 2 packets needed to get the attention of the keyboard in driver
// mode.
extern std::vector<packet> drv_attn;

// It can be useful to send this to keep the keyboard program alive between
// rgb frames.
extern std::vector<packet> drv_idle;

// Set up default settings to talk to keyboard in driver mode.
void init_packet();

#endif
