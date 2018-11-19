This will eventually become a spec for the keyboard driver program.

What do I think I know so far?

Basic USB HID tree info

Driver writes commands to 2nd interface OUT port.  IN port sends
acknowledgement of commands.

I've found that what comes back from the keyboard seems pretty uninteresting,
so all of the following is packets sent from the driver to the keyboard.

Commands are 64 bytes long.  First 6 bytes are command.  Bytes 8-63 are data
payload.  Bytes 6,7 are CRC-16/CCiTT-FALSE, computed with bytes 6,7 set to 0
first.  Then they get replaced with the CRC 16 bit value in little-endian
format.

It appears that the CRC calculation is CRC-16/CCITT-FALSE computed by setting
the crc field to 00 00, then inserting the values swapped.  For example:
1a0201000000a08000000000000000000000000000000000... replacing a080 with 0000
and computing CRC-16/CCiTT-FALSE = 80a0

This works on another packet too:
1a 01 88 01 00 38 aa 4b 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 ff 00 cb 64 ff 00 cb 64 00 00 00 00

Generally, the driver keeps sending
0c0000000000a70d00000000000000000000000000000000... when there's nothing to
do.  It does this about every 0.3 seconds.

# DRIVER MODE PROGRAMMING

I've made progress on the how the driver controls lights in the driver layer.
The driver layer only does anything when the host actively controls it.

Starting the windows driver program kicks the keyboard into driver mode.  I'm
not sure yet what the command is that really does this.

The driver appears to send 3 lighting sequences followed by the 0c00 idle.  A
lighting sequence looks like:

1a0100000038 crc ...
1a0138000038 crc ...
1a0170000038 crc ...
1a01a8000038 crc ...
1a01e0000038 crc ...
1a0118010038 crc ...
1a0150010038 crc ...
1a0188010038 crc ...
1a01c0010038 crc ...
1a01f8010038 crc ...
1a0200000038 crc ...    End of Sequence

The first 10 commands are lighting data per key. The 11th I assume is an end
of sequence command.

Within a command, the 0x38 refers to how many data bytes are valid.

Each of the 1a01 commands is followed by 2 bytes indicating byte counts, I
think.  3800 is little endian representation of 0x0038.  The next command is
0x0070, which is 0x38 more than 0x38.  Each successive command is 0x38 larger
in bytes 2,3 interpreted as little endian 16 bit int.

For a static light pattern, all the sequences would be the same.  Dynamic
patterns would alter the data in the light sequence to animate things.

Within the data section, each group of 4 bytes seems to be:
RRGGBB64
The 0x64 seems to indicate that the key LED has been programmed, rather than
in default state.

## Static light setting and mapping to key LEDs.

Here, each key label or ?? is a group of 4 bytes as above.  ?? indicates I
don't know what this maps to.  Some may refer to keypad keys.  Some probably
refer to the standard PC keys not present on the XBows keyboard, such as
Scroll Lock.  Some likely refer to the various media control and other kinds
of keys.  Take a look at Functions in the windows driver, and look through the
Key base function set choices.  I counted about 140 in total.

1a 01 00 00 00 38 CRC16  Esc		F1
F2			F3     	  	 F4			F5
F6			??		 	 F7  		F8
F9			F10    		 F11	 	F12

1a 01 38 00 00 38 CRC16  Del		??
PrtSc 		??     	  	 ??			??
??			??		 	 `~			1!
2@			??		 	 3#			4$

1a 01 70 00 00 38 c4 a6  5%			??
6^    	 	7&    	     8*			??
9(	 		0)		 	 -_			=+
bksp	 	??		 	 ??			??

1a 01 a8 00 00 38 b7 35	 ??			??
tab   	 	Q     	     W			??
E	 		R		 	 T			XBOW
Y	 		U		 	 I			??

1a 01 e0 00 00 38 c1 42	 O			P
[{    	 	]}    	     \|			pgup
??	 		??		 	 ??			??
caps	 	A		 	 S			??

1a 01 18 01 00 38 b3 86	 D			F
G     	 ctrbksp     	 H			J
K			??		 	 L			;:
'"	 		enter		 ??			pgdn

1a 01 50 01 00 38 c6 d1	 ??			??
??    	 ??    	     	 Lshift		Z
X	 	 ??		 		 C			V
B	 ctrenter	 		 N			M

1a 01 88 01 00 38 d5 82	 ,<			??
.>    	 	/?    	     Rshift		??
up	 		??			??			??
??	 		??			Lctrl		win	

1a 01 c0 01 00 38 d2 c8	 Lalt		??
??    	 	Lspc  	     Mctrl		??
Mshift	 	Rspc		 Ralt		??
??	 		Fn		 	Rctrl		left

1a 01 f8 01 00 18 45 ec	 down		right
??    	 ??    	     	 ??		??
??    	 ??    	     	 ??		??
??    	 ??    	     	 ??		??

1a 20 00 00 00 00 crc	 ??		??



I think this is enough to program light patterns in the driver layer.
