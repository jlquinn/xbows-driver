This will eventually become a spec for the keyboard driver program.

What do I think I know so far?

# How To Reverse Engineer

The approach I'm using is running Windows 7 in a Virtual Box VM on Ubuntu 18.04,
with Wireshark capturing USB packets between the keyboard and the host.

It took a bit of fighting to get a working setup where Windows could see the
keyboard well enough for the driver program to work.

Creating a VM and installing Windows on it was easy.  That step just worked.
However, getting Windows to see the nonstandard USB interfaces of the keyboard
was harder.

One thing I had to do was tell virtualbox to use
/usr/share/virtualbox/VBoxGuestAdditions.iso as a storage device in Windows so
that I could install the windows parts of the USB drivers.

I also had to expose /usr/share/virtualbox-ext-pack to make VirtualBox have
the all the USB bridge drivers necessary.

To get the keyboard exposed, first I had to detach the device from linux so
that virtualbox could expose it to windows:

``
 1144  echo -n '3-11.3:1.1' > /sys/bus/usb/drivers/usbhid/unbind
 1146  echo -n '3-11.3:1.0' > /sys/bus/usb/drivers/usbhid/unbind
 1147  echo -n '3-11.3:1.2' > /sys/bus/usb/drivers/usbhid/unbind
``

At this point, I could install the xbows driver and it could see the keyboard
properly.  Then I could use Wireshark to capture USB packets going back and
forth.



# Basic Device USB HID info

Here is the detailed device descriptor.

```
root@cerberus:/home/jlquinn# lsusb -vd 1EA7:0907

Bus 003 Device 108: ID 1ea7:0907  
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               1.10
  bDeviceClass            0 (Defined at Interface level)
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0        64
  idVendor           0x1ea7 
  idProduct          0x0907 
  bcdDevice            3.00
  iManufacturer           1 SEMITEK
  iProduct                2 USB-HID Gaming Keyboard
  iSerial                 3 SN0000000001
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength           91
    bNumInterfaces          3
    bConfigurationValue     1
    iConfiguration          0 
    bmAttributes         0xa0
      (Bus Powered)
      Remote Wakeup
    MaxPower              100mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      1 Boot Interface Subclass
      bInterfaceProtocol      1 Keyboard
      iInterface              0 
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength      64
          Report Descriptor: (length is 64)
            Item(Global): Usage Page, data= [ 0x01 ] 1
                            Generic Desktop Controls
            Item(Local ): Usage, data= [ 0x06 ] 6
                            Keyboard
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Global): Usage Page, data= [ 0x07 ] 7
                            Keyboard
            Item(Local ): Usage Minimum, data= [ 0xe0 ] 224
                            Control Left
            Item(Local ): Usage Maximum, data= [ 0xe7 ] 231
                            GUI Right
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Global): Report Count, data= [ 0x08 ] 8
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Main  ): Input, data= [ 0x03 ] 3
                            Constant Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0x03 ] 3
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Global): Usage Page, data= [ 0x08 ] 8
                            LEDs
            Item(Local ): Usage Minimum, data= [ 0x01 ] 1
                            NumLock
            Item(Local ): Usage Maximum, data= [ 0x03 ] 3
                            Scroll Lock
            Item(Main  ): Output, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x05 ] 5
            Item(Main  ): Output, data= [ 0x03 ] 3
                            Constant Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0x06 ] 6
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xa4 0x00 ] 164
            Item(Global): Usage Page, data= [ 0x07 ] 7
                            Keyboard
            Item(Local ): Usage Minimum, data= [ 0x00 ] 0
                            No Event
            Item(Local ): Usage Maximum, data= [ 0xa4 ] 164
                            ExSel
            Item(Main  ): Input, data= [ 0x00 ] 0
                            Data Array Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0008  1x 8 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           2
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 No Subclass
      bInterfaceProtocol      0 None
      iInterface              0 
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength      34
          Report Descriptor: (length is 34)
            Item(Global): Usage Page, data= [ 0x00 0xff ] 65280
                            (null)
            Item(Local ): Usage, data= [ 0x50 ] 80
                            (null)
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x00 ] 255
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x40 ] 64
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Local ): Usage, data= [ 0x03 ] 3
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x00 ] 255
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x40 ] 64
            Item(Main  ): Output, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x83  EP 3 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x04  EP 4 OUT
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        2
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 No Subclass
      bInterfaceProtocol      0 None
      iInterface              0 
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.11
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength     203
          Report Descriptor: (length is 203)
            Item(Global): Usage Page, data= [ 0x01 ] 1
                            Generic Desktop Controls
            Item(Local ): Usage, data= [ 0x80 ] 128
                            System Control
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Global): Report ID, data= [ 0x01 ] 1
            Item(Local ): Usage Minimum, data= [ 0x81 ] 129
                            System Power Down
            Item(Local ): Usage Maximum, data= [ 0x83 ] 131
                            System Wake Up
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x01 ] 1
            Item(Global): Report Count, data= [ 0x03 ] 3
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x05 ] 5
            Item(Main  ): Input, data= [ 0x01 ] 1
                            Constant Array Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Global): Usage Page, data= [ 0x0c ] 12
                            Consumer
            Item(Local ): Usage, data= [ 0x01 ] 1
                            Consumer Control
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Global): Report ID, data= [ 0x02 ] 2
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x01 ] 1
            Item(Global): Report Count, data= [ 0x12 ] 18
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Local ): Usage, data= [ 0x83 0x01 ] 387
                            AL Consumer Control Configuration
            Item(Local ): Usage, data= [ 0x8a 0x01 ] 394
                            AL Email Reader
            Item(Local ): Usage, data= [ 0x92 0x01 ] 402
                            AL Calculator
            Item(Local ): Usage, data= [ 0x94 0x01 ] 404
                            AL Local Machine Browser
            Item(Local ): Usage, data= [ 0xcd ] 205
                            Play/Pause
            Item(Local ): Usage, data= [ 0xb7 ] 183
                            Stop
            Item(Local ): Usage, data= [ 0xb6 ] 182
                            Scan Previous Track
            Item(Local ): Usage, data= [ 0xb5 ] 181
                            Scan Next Track
            Item(Local ): Usage, data= [ 0xe2 ] 226
                            Mute
            Item(Local ): Usage, data= [ 0xea ] 234
                            Volume Decrement
            Item(Local ): Usage, data= [ 0xe9 ] 233
                            Volume Increment
            Item(Local ): Usage, data= [ 0x21 0x02 ] 545
                            AC Search
            Item(Local ): Usage, data= [ 0x23 0x02 ] 547
                            AC Home
            Item(Local ): Usage, data= [ 0x24 0x02 ] 548
                            AC Back
            Item(Local ): Usage, data= [ 0x25 0x02 ] 549
                            AC Forward
            Item(Local ): Usage, data= [ 0x26 0x02 ] 550
                            AC Stop
            Item(Local ): Usage, data= [ 0x27 0x02 ] 551
                            AC Refresh
            Item(Local ): Usage, data= [ 0x2a 0x02 ] 554
                            (null)
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x0e ] 14
            Item(Main  ): Input, data= [ 0x01 ] 1
                            Constant Array Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Global): Usage Page, data= [ 0x01 ] 1
                            Generic Desktop Controls
            Item(Local ): Usage, data= [ 0x06 ] 6
                            Keyboard
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Global): Report ID, data= [ 0x04 ] 4
            Item(Global): Usage Page, data= [ 0x07 ] 7
                            Keyboard
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Main  ): Input, data= [ 0x03 ] 3
                            Constant Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Report Count, data= [ 0xe8 ] 232
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x01 ] 1
            Item(Global): Usage Page, data= [ 0x07 ] 7
                            Keyboard
            Item(Local ): Usage Minimum, data= [ 0x00 ] 0
                            No Event
            Item(Local ): Usage Maximum, data= [ 0xe7 ] 231
                            GUI Right
            Item(Main  ): Input, data= [ 0x00 ] 0
                            Data Array Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Global): Usage Page, data= [ 0x01 ] 1
                            Generic Desktop Controls
            Item(Local ): Usage, data= [ 0x02 ] 2
                            Mouse
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Global): Report ID, data= [ 0x05 ] 5
            Item(Local ): Usage, data= [ 0x01 ] 1
                            Pointer
            Item(Main  ): Collection, data= [ 0x00 ] 0
                            Physical
            Item(Global): Usage Page, data= [ 0x09 ] 9
                            Buttons
            Item(Local ): Usage Minimum, data= [ 0x01 ] 1
                            Button 1 (Primary)
            Item(Local ): Usage Maximum, data= [ 0x08 ] 8
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x01 ] 1
            Item(Global): Report Count, data= [ 0x08 ] 8
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Usage Page, data= [ 0x01 ] 1
                            Generic Desktop Controls
            Item(Local ): Usage, data= [ 0x30 ] 48
                            Direction-X
            Item(Local ): Usage, data= [ 0x31 ] 49
                            Direction-Y
            Item(Global): Logical Minimum, data= [ 0x01 0xf8 ] 63489
            Item(Global): Logical Maximum, data= [ 0xff 0x07 ] 2047
            Item(Global): Report Count, data= [ 0x02 ] 2
            Item(Global): Report Size, data= [ 0x0c ] 12
            Item(Main  ): Input, data= [ 0x06 ] 6
                            Data Variable Relative No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Local ): Usage, data= [ 0x38 ] 56
                            Wheel
            Item(Global): Logical Minimum, data= [ 0x81 ] 129
            Item(Global): Logical Maximum, data= [ 0x7f ] 127
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Main  ): Input, data= [ 0x06 ] 6
                            Data Variable Relative No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Global): Usage Page, data= [ 0x0c ] 12
                            Consumer
            Item(Local ): Usage, data= [ 0x38 0x02 ] 568
                            AC Pan
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Main  ): Input, data= [ 0x06 ] 6
                            Data Variable Relative No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Main  ): End Collection, data=none
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x82  EP 2 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               1
Device Status:     0x0000
  (Bus Powered)
root@cerberus:/home/jlquinn# 
```

The first interface is the standard USB HID keyboard interface.

Driver writes commands to 2nd interface OUT port.  IN port sends
acknowledgement of commands.

I *think* that the 3rd interface supports the keyboard generating mouse
keyclicks and some media keys.


# Commands from Host to XBows

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

```
This works on another packet too:
1a 01 88 01 00 38 aa 4b 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 ff 00 cb 64 ff 00 cb 64 00 00 00 00
```

Generally, the driver keeps sending
0c0000000000a70d00000000000000000000000000000000... when there's nothing to
do.  It does this about every 0.3 seconds.

# Driver Mode Programming

I've made progress on the how the driver controls lights in the driver layer.
The driver layer only does anything when the host actively controls it.

Starting the windows driver program kicks the keyboard into driver mode.  I'm
not sure yet what the command is that really does this.

The driver appears to send 3 lighting sequences followed by the 0c00 idle.  A
lighting sequence looks like:

```
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
```

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
```
RRGGBB64
```
The 0x64 seems to indicate that the key LED has been programmed, rather than
in default state.

## Static light setting and mapping to key LEDs.

Here, each key label or ?? is a group of 4 bytes as above.  ?? indicates I
don't know what this maps to.  Some may refer to keypad keys.  Some probably
refer to the standard PC keys not present on the XBows keyboard, such as
Scroll Lock.  Some likely refer to the various media control and other kinds
of keys.  Take a look at Functions in the windows driver, and look through the
Key base function set choices.  I counted about 140 in total.

```
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
```


I think this is enough to program light patterns in the driver layer.

## Modifying Key Mappings in Driver Mode

I think that we have a sequence that looks like this to set the key bindings
in driver mode.

```
0b0500000000 crc 00000000 x 14
010900000000 crc 00000000 x 14
160100000038 crc ffffffff ffffffff
				 01000002 02000002 04000002 08000002	Ls Lc La Win
	     		 10000002 20000002 40000002 ffffffff	Rc Rs Ra
				 00040002 00050002 00060002 00070002	A B C D
160138000038 crc 00080002 00090002						E F
				 000a0002 000b0002 000c0002 000d0002	G H I J
	     		 000e0002 000f0002 00100002 00110002	K L M N
				 00120002 00130002 00140002 00150002	O P Q R
160170000038 crc 00160002 00170002						S T
				 00180002 00190002 001a0002 001b0002	U V W X
	     		 001c0002 001d0002 001e0002 001f0002	Y Z 1 2
				 00200002 00210002 00220002 00230002	3 4 5 6
1601a8000038 crc 00240002 00250002						7 8
				 00260002 00270002 00280002 00290002	9 0 Renter Esc
				 002a0002 002b0002 002c0002 002d0002	BS Tab Lspc -_
				 002e0002 002f0002 00300002 00310002	=+ [{ ]} \|
1601e0000038 crc 00330002 00340002						;: '"
				 00350002 00360002 00370002 00380002	`~ ,< .> /?
				 00390002 003a0002 003b0002 003c0002	Caps F1 F2 F3
				 003d0002 003e0002 003f0002 00400002	F4 F5 F6 F7
160118010038 crc 00410002 00420002						F8 F9
				 00430002 00440002 00450002 00460002	F10 F11 F12 PrtScrn
	     		 ffffffff x 4
				 004b0002 004c0002 ffffffff 004e0002	PgU Del ?? PgD
160150010038 crc 004f0002 00500002						Rt Lt
				 00510002 00520002						Dn Up
				 ffffffff x 10
160188010038 crc ffffffff x 14
1601c0010020 crc 002a0002 00280002						Mbs Menter
				 01000002 20000002 002c0002				Mc  Ms     Rspc
	     		 ffffffff x 3
				 00000000 x 6
160300000038 crc ffffffff x 14
160338000038 crc ffffffff x 14
160370000008 crc ffffffff ffffffff 00000000 x 12
170100000000 crc f0011203 00000000 0f000000 00000000 x 11
0c0000000000 crc end of sequence code
```

Here, the first 2 commands 0b05 and 0109 appear to the start of the remapping
commands.

The data commands show the keys they correspond to.  Remapping involves
putting a different key's code in that spot.  For example, to remap Esc to Q,
the 00290002 value would be changed to 00140002.

I assume that the various unlabeled ints are just part of the programming.
Some might be the numeric keypad but I don't have one, so I can't test that
theory for now.

Commands 1603 and 1701 appear to just terminate the programming sequence.


# Custom Layer Programming

This section contains notes on how programming the XBows custom layers works.

Here is a trace of the driver programming the lights in a custom layer.

```
010900000000 crc 00
210301000000 crc 00
220300003800 crc ffffffff ffffffff 01000002 02000002 04000002 08000002
	     	 10000002 20000002 00630002 ffffffff 00040002 00050002
		 00060002 00070002 pos 8 different
220338003800 crc 00080002 00090002
	     	 000a0002 000b0002 00600002 005c0002	pos 4, 5, 6, 7, 8 10,11 different
		 005d0002 005e0002 00590002 00110002
		 00610002 00560002 00140002 00150002	different than CUST1
		 THIS MAY BE THE KEYPAD OVERLAY!!!!
220370003800 crc 00160002 00170002
	     	 005f0002 00190002 001a0002 001b0002    pos 2 is different
		 001c0002 001d0002 001e0002 001f0002
		 ... 00230002
2203a8003800 crc 00240002 00540002
	     	 00550002 00270002 thru 00310002	pos 1,2 different
2203e0003800 crc 00570002 00340002 			pos 0,3,4 different
	     	 00350002 005a0002 005b0002 00580002
		 00390002 thru 00400002
220318013800 crc 00410002 ... 00460002 (6 ints)
	     	 ffff (4 ints) these line up with missing expected ints
		 004b0002 004c0002 ffff 004e0002  	  SAME AS 160118...
220350013800 crc same as 160150010038
220388013800 crc ff x 14 same as 160188010038
2203c0012000 crc 002a0002 00280002 01000002 20000002 00620002
	     	 ffffffff (3 ints)
		 00 (6 ints)  NOT same as 1601c0010020  pos 4 different
	     	 lighting program starts here?
210304000000 crc 0000 (14 ints)
210305000000 crc 0000 (14 ints)
260300003800 crc ffff (14 ints)
260338003800 crc ffff (14 ints)
260370000800 crc ffff ffff 00 (12 ints)
210306000000 crc 00 x 14
270300000038 crc 00020000 03000000
	     	 4e020000 01000000 ff x 10
270338000038 crc ff x 14
270370000038 crc ff x 14
2703a8000038 crc ff x 14
2703e0000038 crc ff x 14	packet 467
270318010038 crc ff x 14
270350010038 crc ff x 14
270388010038 crc ff x 14
2703c0010038 crc ff x 14
2703f8010038 crc ff ff
	     	 03001600 ff x 3
		 ffffffff 0f000000 00000300 1600ffff
		 ff x 3   	   	    ffff0f00
270330020038 crc 00000000 03001600
	     	 ff x 4
		 0f000000 00000020 ff x 2
		 ff x 2	  	   0f000000 0000ff00
270368020006 crc 00000000 79000000
	     	 00 x 12
0b0300000000 crc 00 x 14
0c0000000000 	      at 3.0737sec
```

The programming sequence starts with the 0109 command.  This shows up in the
driver layer programming as well, but here it's the first packet.

Remaining packets encode the custom layer being programmed in byte 1.  For
example:

`
270368020006
`

Here, byte 1 is 0x03.  0x02 addresses custom layer 1, 0x03 is layer 2, and
0x04 is layer 3.

Command 2 is 0x21.  This is followed by a bunch of 0x22 commands.  The 0x22
commands affect the keyboard mapping for the custom layer.

After 9 packets of 0x22 commands, there is a 6 packet sequence of 0x21 0x21
0x26 0x26 0x26 0x21.  I don't know what this does other than separate keyboard
from lighting.

Programming the lights happens with 0x27 commands.  This sequence is variable
length.

The sequence is terminated with the 0x0b command.

## Custom Layer Keyboard Programming



## Custom Layer Light Programming

The DIY Lights section of the driver enables writing new patterns.  The driver
explains a few things to us.


The driver assembles patterns from two building blocks: animation frames, and
lighting frames.  

Animation frames consist of a duration and a set of active keys.

Light frames are a little more complex.  They have a duration and a set of
active keys.  Lights can have one of three patterns: monochrome, RGB cycle,
and breathing.  The light pattern also has a starting color.

These features appear to show up in the USB programming model as well.

