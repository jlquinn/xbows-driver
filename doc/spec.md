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

See the following for details:
http://www.slashdev.ca/2010/05/08/get-usb-report-descriptor-with-linux/
describes how to unbind usb device to get report descriptors


## Wireshark setup

modprobe usbmon

On ubuntu 18.04, I can't get wireshark to see usbmon3 as a regular user.  The
faq says to run

sudo setfacl -m u:$USER:r /dev/usbmon3

but this doesn't change anything.  Root can see it.


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

# Switching layers

The windows driver can tell the keyboard to switch between layers.  Looking
for this control packet, I find:

```
0b 03 00 00 00 00 crc 0000 x 14
```

The response is the same as the command.

0b02 enables layer 1.
0b03 enables layer 2.
0b04 enables layer 3.
0b05 enables driver mode layer.


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

1a 02 00 00 00 00 crc	 ??		??
```


When sending 1a command packets, each modified gets RRGGBBxx, where xx is the
key mode, I think.  If the program is numpad in green, I see keys assigned to
00FF0064 if they are lit.

1a02 appears to mark the end of the lighting sequence.  The Windows driver
keeps sending lighting frames actively when running, whether the pattern is
dynamic or not.

When the program is shifted to blue lights breathing, I see 0000FF64 for each
lit key.  Values assigned to the blue field to emulate breathing:

	ff ff e5 cc b2 99 7f 66 4c 33 19 00 00
	00 00 19 33 4c 66 7f 99 b2 cb e5 fe ff

You can see some roundoff happening near the end of the cycle.

So for driver mode, breathing isn't a hardware setting the way it is for
custom mode.

Therefore, I don't know what the 64 at the end of the RGB indicates.

Checking with an RGB cycle, I see value sequences

	Blue 00ffBB64
	0c 19 26 33 3f 4c 59 65 72 7f 8c 99 a5 b2 bf cb d8 e5 f2 ff
	Green 00GGff64
	f2 e5 d8 cb bf b2 a5 99 8c 7f 72 65 59 4c 3f 33 26 19 0c 00
	Red   RR00ff64
	0c 19 26 33 3f 4c 59 65 72 7f 8c 99 a5 b2 bf cb d8 e5 f2 ff
	Blue  ff00BB64	
	f2 e5 d8 cb bf b2 a5 99 8c 7f 72 65 59 4c 3f 33 26 19 0c 00
	Green ffGG0064
	0c 19 26 33 3f 4c 59 65 72 7f 8c 99 a5 b2 bf cb d8 e5 f2 ff


I think this is enough to program light patterns in the driver layer.

After sending 1a01 command packets, the keyboard responds with:
1a0101000000a4b400000000000000000000000000000000...

After sending 1a02, the keyboard responds with 
1a0201000000a08000000000000000000000000000000000...

Windows driver sends each lighting frame every ~0.3 seconds.  I don't know how
fast it could actually be done.

20190117 I tried sending a lighting frame.  It appears to just block on the
first command.

OK, after looking deeper, it seems that the windows driver updates a light
program by sending a keymap, followed by 0x0c, followed by an updated light
program.

With experimentation, we need to send the 0b05 and 0109 commands to get the
keyboard's attention.  But we don't have to send a keymap, just a light frame.

Once we have the attention of the keyboard, we can just keep sending packets
periodically.



## Modifying Key Mappings in Driver Mode

I think that we have a sequence that looks like this to set the key bindings
in driver mode.

```
0b0500000000 crc 00000000 x 14
010900000000 crc 00000000 x 14
160100000038 crc ffffffff ffffffff
				 01000002 02000002 04000002 08000002	Lc Ls La Win
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

XXX Check Mctl.  It is probably 10000002, not 01000002, same as Rctl.

Here, the first 2 commands 0b05 and 0109 appear to the start of the remapping
commands.

The data commands show the keys they correspond to.  Remapping involves
putting a different key's code in that spot.  For example, to remap Esc to Q,
the 00290002 value would be changed to 00140002.

I assume that the various unlabeled ints are just part of the programming.
Some might be the numeric keypad but I don't have one, so I can't test that
theory for now.

Commands 1603 and 1701 appear to just terminate the programming sequence.
1603 has 120 0xff bytes.

After sending a command to the keyboard, the keyboard sends a response.

After 0b05, the response is identical.
After 0109, the response is:
010901000000cf3d0f080000000000000000000000000000...
I don't know what the 0f08 indicates yet.

After each 0x16 command, I see this as a response:
160101000000d96f00000000000000000000000000000000...

After the basic keymap is sent, the next 3 packets are cmd 0x1603.  I don't
understand them yet.  The replies look like:
160301000000214800000000000000000000000000000000...

The last packet is command 0x1701.  I don't understand this yet.  The response is:
170100000000fe51f0011203000000000000000000000000...


QUESTIONS:

How do macros, flashlight, and shortcuts get implemented in driver mode?

How does a firmware update get sent to the keyboard?

Do I need to interpret the keyboard return packets?

### FnX mapping

Assigning an FnX mapping to a key shows up in the driver keymap.  Assigning
LCtrl to Custom Layer1 gives 

```
160100000038 crc ffffffff ffffffff
				 0200070a 02000002 04000002 08000002	Lc Ls La Win
	     		 10000002 20000002 40000002 ffffffff	Rc Rs Ra
				 00040002 00050002 00060002 00070002	A B C D
```

With LCtrl slot set to 0x0200070a.  Setting to Custom Layer 2 gives
0x0300070a.  Custom Layer 3 gives 0x0400070a.  Setting to Standard (driver
layer) gives 0x0100070a.





## Driver Mode flashlight

Driver mode provides what is called flashlight settings.  When a key is
assigned a flashlight, pressing the key causes a light pattern to run briefly.

There appears to be a change in the keymap program when this happens.  The
keymap program has a section with 1603 commands.  There are changes here.  By
default, I see the following when the driver keymap is programmed:

```
160300000038 crc ffffffff x 14
160338000038 crc ffffffff x 14
160370000008 crc ffffffff ffffffff 00000000 x 12
```

If I assign 'S' to have calculator, I see:

```
160300000038 crc ffffffff ffffffff
	ffff x 4
	ffffffff 1dffffff ffff x 6
160338000038 crc ffffffff x 14
160370000008 crc ffffffff ffffffff 00000000 x 12
```

Note the 0x1d at data byte 29.  So far this doesn't match another
representation of S.

If I assign LShift to Flow, I see:
```
160300000038 crc ffffff04 ffffffff
	ffff x 4
	ffffffff 1dffffff ffff x 6
160338000038 crc ffffffff x 14
160370000008 crc ffffffff ffffffff 00000000 x 12
```

data byte 4 is set to 0x04.  Changing LShift to Calculator doesn't change the
bit pattern.  This suggests that the driver program remembers the assignment.
The keyboard merely informs the driver to initiate the light program.

When I press the LShift key, I see a single frame of the Calculator light
program sent to the keyboard.

This could easily be done with something that monitors the keyboard for the
specified keys.  But there must be some reason the keymap gets modified.

Position mapping appears to be the same as keymap:

	LCtrl 160300000038 crc ffff03ff ...
	LShft 160300000038 crc ffffff04 ...
	LAlt  160300000038 crc ffffffff 05ffffff ...
	Win   160300000038 crc ffffffff ff06ffff ...
	RCtrl 160300000038 crc ffffffff ffff07ff ...
	RShft 160300000038 crc ffffffff ffffff08 ...
	RAlt  160300000038 crc ffffffff x 2 09ffffff ....
	A     160300000038 crc ffffffff x 2 ffff0bff ....
	B     160300000038 crc ffffffff x 2 ffffff0c ....
	C     160300000038 crc ffffffff x 3 0dffffff ....

Each byte also appears to contain the number of its position.

IMPORTANT: I discovered that the driver sends 0x18 commands to keyboard with
flashlight setting.  We assigned a calc flashlight to S in position 0x1d.

0040  18 04 1d 01 00 00 68 c1 00 00 00 00 00 00 00 00   ......h.........
0050  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0060  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0070  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................

Returns 18041d000000 crc

I still don't see how this programs in calculator.

I also haven't see this show up regularly when assigning flashlight.

I applied Full White Lighting to key C.  Pressing the C key causes the driver
to send 3 frames of all white light to the keys.

So flashlight appears to be implemented at the driver.  Except that the 1603
commands record something about flashlight being assigned.

When C is pressed, I see the key on endpoint 3.110.1.


## Driver Mode macro assignment

Testing how a macro gets assigned to a key in driver mode.  I apply the macro
12345 to the key LCtrl.  This gives the first command:

```
160100000038 crc ffffffff ffffffff
				 0300010a 02000002 04000002 08000002	Lc Ls La Win
	     		 10000002 20000002 40000002 ffffffff	Rc Rs Ra
				 00040002 00050002 00060002 00070002	A B C D
```

The LCtrl key position gets 0300010a instead of 01000002.
I don't know how the keyboard is remembering the macro, however.  Or if the
driver is injecting the key sequence in place of the keyboard.

IMPORTANT: I just discovered the driver sends 0x15 commands to the keyboard.
I don't know if this stores macro/flashlight info or what yet.

pkt 882
```
1503000000 crc 01000000 0000 x 13
```

pkt 891
```
1502000000 crc 001e0000 0000 x 13
```

pkt 981
```
1502000000 crc 00000000 x 14
```
pkt 1035
```
1502000000 crc 001f0000 0000 x 13
```
pkt 1085
```
1502000000 crc 00000000 x 14
```
pkt 1135
```
1502000000 crc 00200000 0000 x 13
```
pkt 1187
```
1502000000 crc 00000000 x 14
```
pkt 1241
```
1502000000 crc 00210000 0000 x 13
```
pkt 1291
```
1502000000 crc 00000000 x 14
```
pkt 1347
```
1502000000 crc 00220000 0000 x 13
```
pkt 1405
```
1502000000 crc 00000000 x 14
```
pkt 1415
```
1503000000 crc 00000000 x 14
```
pkt 1417
```
1503000000 crc 00000000 x 14
```
pkt 1423
```
1501000000 crc 00000000 x 14
```
pkt 1433
```
1502000000 crc 00000000 x 14
```

It appears that key codes for 12345 are present in the above sequence.  Also,
the 0x15 command sequence is interleaved with lighting commands.

My suspicion is that the leading 1503 with 01 after the crc starts the macro,
and 01 is the macro number.  We have 1503 1503 1501 1502 at the tail end.

Q: are all 4 commands needed to terminate the macro?
Q: Are 1502 with 0 the interkey delays for the macro?

The actual macro has variable delays between down and up key events.  This
sequence shows the keys, but doesn't seem to separate down key and up key
events, or encode the delays.

Assigning C to the 12345 macro puts 0d00010a in place of 00060002.  The 0x15
command sequence begins ~0.3 sec after the 0x16 keymap command sequence.

Adding B as the long macro puts 0c00010a in place of 00050002.  Here the macro
command sequence starts 3 seconds after keymap!

Q: what is the significance of the 0d and 0c?
A: This is the position in the keymap.  So B is position 12 (0xc) in the 0x1601
keymap counting from 1.

```
1503000000 crc 01000000 00 x 13
1501000000 crc 01000000 00 x 13
1501000000 crc 00 x 14
1501000000 crc 02000000 00 x 13
1501000000 crc 02000000 00 x 13
1501000000 crc 00 x 14
1502000000 crc 001f0000 00 x 13
1502000000 crc 00 x 14
1502000000 crc 00 x 14
1502000000 crc 00200000 00 x 13
1502000000 crc 00 x 14
0c
1501000000 crc 01000000 00 x 13
1501000000 crc 00 x 14
1502000000 crc 00210000 00 x 13
0c
1502000000 crc 00 x 14
1502000000 crc 00220000 00 x 13
1502000000 crc 00 x 14
0c
1502000000 crc 00230000 00 x 13
1502000000 crc 00 x 14
1502000000 crc 00240000 00 x 13
0c
1502000000 crc 00 x 14
1502000000 crc 00250000 00 x 13
1502000000 crc 00 x 14
0c
1502000000 crc 00260000 00 x 13
...
```


It appears that only one macro can be assigned to a key at a time.  I don't
know if this is a keyboard limitation or driver limitation.  Since the macro
doesn't seem to have a way to indicate that it is associated with a specific
key, or the key with a specific macro, I think the one macro at a time is a
keyboard design, i.e. when you add a macro, the firmware sees which key just
got a macro added, and matches the next macro to that key.

It's also possible the keyboard remembers a macro that has already been
stored.  I can't quite tell.

This appears to use the first 2 data bytes to put a key code from the
driver/custom key id map.

This is not quite right.  If I program a macro sequence LShft down, Z down, Z
up, LShft up, I see the macro programming as follows (0c given for timing):

```
1503000000 crc 01000000 00 x 13
0c
1502000000 crc 02000000 00 x 13
0c
0c
1502000000 crc 021d0000 00 x 13
0c
1502000000 crc 02000000 00 x 13
0c
0c
1502000000 crc 00 x 14
1503000000 crc 01000000 00 x 13
1503000000 crc 01000000 00 x 13
1501000000 crc 00 x 14
1502000000 crc 00 x 14
```

0x021d indicates Lshift and Z pressed at the same time.  So the key id table
is a bit more complex than I thought.

Macros in driver mode record mouse clicks different from custom layer.  I programmed
the following macro sequence: Lclick, Lclick up, LShift down, Z down, Z up,
LShift up, Rclick, Rclick up.  I see the following commands:


```
1503000000 crc 01000000 00 x 13
1501000000 crc 01000000 00 x 13
1501000000 crc 00000000 00 x 13
1502000000 crc 02000000 00 x 13
1502000000 crc 021d0000 00 x 13
1502000000 crc 02000000 00 x 13
1501000000 crc 02000000 00 x 13
1501000000 crc 00000000 00 x 13
1503000000 crc 01000000 00 x 13
1501000000 crc 00 x 14
1502000000 crc 00 x 14
```

Here, the first byte of Lclick is 01, and the first byte of Rclick is 02.  The
0101 portion of 01000101 and 02000101 appears to be ignored.

Q: is the 0101 portion of mouse click buttons in the key id map spurious and wrong?




## Driver Mode must stay active

It turns out that to maintain an active light program, the driver must send
packets periodically.  If not, the keyboard will revert to the last used
built-in backlight mode after about 3 seconds.

This is also true for key assignments.  The keymap reverts to default after
about 3 seconds.  This suggests that refreshing the keyboard with a lighting
program will preserve the keymap.

Sending light frames after the keymap is what the driver does to keep things
going.

Q: Is sending 0c periodically enough?
A: Yes.  We don't have to send light frames to keep the driver program alive.
Just sending an idle command every 2 seconds is adequate.



# Custom Layer Programming

This section contains notes on how programming the XBows custom layers works.

Here is a trace of the driver programming the lights in a custom layer.

```
010900000000 crc 00
210301000000 crc 00
220300003800 crc ffffffff ffffffff
			 	 01000002 02000002 04000002 08000002
	     	 	 10000002 20000002 00630002 ffffffff
			 	 00040002 00050002 00060002 00070002 pos 8 different
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

Question: if the keyboard were programmed with 2201 commands, would this
permanently write to the driver layer?

## Custom Layer Keyboard Programming

As mentioned above, the sequence of 0x22 commands remaps the keyboard of a
custom layer.

Bytes 2 and 3 form a 16 bit little-endian int representing the index of the
data bytes give so far (I think).

Byte 4 gives the number of valid data bytes.  Most packets will have 0x38
here.  The last one may be less.

I *think* that the 0x22 command sequence is the same as the 0x16 command
sequence that programs the keymap in the driver layer.  I don't know for sure
yet.

In the 0x2203 sequence for custom layer 2, the default configuration has a
numeric keypad programmed in, which might explain why packet 220300003800 int
8 is 00630002 instead of 40000002.


```
010900000000 crc 00
210201000000 crc 00
220200003800 crc ffffffff ffffffff
			 	 01000002 02000002 04000002 08000002
	     	 	 10000002 20000002 40000002 ffffffff
			 	 00040002 00050002 00060002 00070002
220238003800 crc 00080002 00090002
	     	 	 000a0002 000b0002 000c0002 000d0002
			 	 000e0002 000f0002 00100002 00110002
			 	 00120002 00130002 00140002 00150002
220270003800 crc 00160002 00170002
	     	 	 00180002 00190002 001a0002 001b0002    pos 2 is different
			 	 001c0002 001d0002 001e0002 001f0002
			 	 ... 00230002
2202a8003800 crc 00240002 00250002
	     	 	 00260002 00270002 00280002 003a0002	3a is F1 vs 29 is Esc
				 002a0002 002b0002 002c0002 002d0002
				 002e0002 002f0002 00300002 00310002
2202e0003800 crc 00330002 00340002
	     	 	 00350002 00360002 00370002 00380002
			 	 00390002 thru 00400002
220218013800 crc 00410002 ... 00460002 (6 ints)
	     	 	 ffff (4 ints) these line up with missing expected ints
			 	 004b0002 004c0002 ffffffff 004e0002
220250013800 crc 004f0002 00500002
				 00510002 00520002
	     	 	 ffff (10 ints) these line up with missing expected ints
220288013800 crc ff x 14 same as 160188010038
2202c0012000 crc 002a0002 00280002
				 01000002 20000002 002c0002
	     		 ffffffff (3 ints)
				 00 x 6							ignored due to 0x20 in packet byte 4
... lighting program here ...
0b0200000000 crc 00 x 14
0c0000000000 	      at 3.0737sec
```

Command 0x0109 tells the keyboard that a keymap is coming, followed by lighting.

## Disabling a key

The Windows driver provides the ability to disable a key.  This is handled by
setting the code for a key to 00.  For example, Esc is 0x29 and the code would
normally be 0x00290002.  Disabling the Esc key would instead generate
0x00000002.

## Flashlight

The flashlight feature appears to be programmed into the set of commands
starting with 0x26.  These normally have 0xff in them.  

Before, there was a fixed sequence between the keymap and lighting program:

```
210204000000 crc 0000 (14 ints)
210205000000 crc 0000 (14 ints)
260200003800 crc ffff (14 ints)
260238003800 crc ffff (14 ints)
260270000800 crc ffff ffff 00 (12 ints)
210206000000 crc 00 x 14
```

Setting Q to flash the
windmill sequence gives the following:


```
210204000000 crc 0000 (14 ints)
210205000000 crc 0000 (14 ints)
260200003800 crc ffffffff ffffffff
	ffffffff ffffffff ffffffff ffffffff
	ffff01ff ffffffff ffffffff ffffffff
	ffffffff ffffffff ffffffff ffffffff
260238003800 crc ffff (14 ints)
260270000800 crc ffff ffff 00 (12 ints)
210206000000 crc 00 x 14
```

Setting A to flash windmill gives 

```
260200003800 crc ffffffff ffffffff
	ffff01ff ffffffff ffffffff ffffffff
	ffffffff ffffffff ffffffff ffffffff
	ffffffff ffffffff ffffffff ffffffff
```

Changing the lighting program that is flashed doesn't affect the change from
0xff to 0x01.

QUESTION: where is this info stored?

If Q is set to windmill, and A is set to gradual brighten, I get the
following:

```
260200003800 crc ffffffff ffffffff
	ffff01ff ffffffff ffffffff ffffffff
	ffff02ff ffffffff ffffffff ffffffff
	ffffffff ffffffff ffffffff ffffffff
```

It looks like the byte is an index into a table of light programs.

QUESTION: where are the lighting programs stored?


Slightly different experiment.  Layer 3.  General lighting program is
Calculator1 which is 20 animation frames with only Esc set, and 2 frames of
lighting.  The first lighting frame is duration 5, RGB ff0000 (all red)
monochrome style, only Esc set.  The second lighting frame is duration 1, RGB
0900ff (almost blue) monochrome style, only Esc set.

Since both monochrome styles are active at the same time, it's possible only
the 2nd one is delivered to the keyboard.

In addition, I set Q with a flashlight of Calculator11.  Calc11 is 2 animation
frames, frame 1 with Esc set, frame 2 with F1 set.  2 lighting frames.
Lighting frame 1 is duration 7, RGB ff00dd monochrome, Esc and F1 set.
Lighting frame 2 is duration 2, RGB 00ffee RGB style, Esc and F1 set.

Labeling of Q is the same as the first test.  We get 01 on the 27th byte.
This seems to include 6 bytes before the A byte.

QUESTION: does this match the following key code mapping?  It holds for alpha
keys so far.


Dipping into lighting info...  If I have just calculator1 chosen as the
layer lighting program, the lighting looks like:


```
270400000038 crc  00020000 14000000		start of lighting program
08040000 02000000 ffffffff ffffffff		
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
```

00020000 starts the lighting program
14000000 there are 20 animation frames
08040000 appears to be 0x1a * 20 + 0200
02000000 total lighting program duration is 2
0xffffffff  there are 124 of these
03001600 0100...  one animation frame with Esc set.  22 byte bitmap following
	the frame header.  There are 20 of these.
00200100 0020 starts lighting frame 1.  01 is the start of the
	22 byte bitmap for lighting frame 1.
ff000000 RRGGBB00 for this frame
00003400 monochrome lighting style?
00200100 0020 starts lighting frame 2.  01 is the start of the
	22 byte bitmap for lighting frame 2.
0900ff00 RRGGBB00 for this frame
00003400 monochrome lighting style?


Now assign flashlight to Q with Full white lighting.  Reminder, there are 3
animation frames.



```
270400000038 crc  00020000 14000000		start of lighting program
08040000 02000000 48040000 03000000
96040000 01000000 ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
```

00020000 starts the lighting program
14000000 there are 20 animation frames
08040000 appears to be 0x1a * 20 + 0200
02000000 total lighting program duration is 2
48040000 03000000 96040000 01000000  I suspect these are key assignments of

some sort
0xffffffff  there are 120 of these
03001600 0100...  one animation frame with Esc set.  22 byte bitmap following
	the frame header.  There are 20 of these.
00200100 0020 starts lighting frame 1.  01 is the start of the
	22 byte bitmap for lighting frame 1.
ff000000 RRGGBB00 for this frame
00003400 monochrome lighting style?
00200100 0020 starts lighting frame 2.  01 is the start of the
	22 byte bitmap for lighting frame 2.
0900ff00 RRGGBB00 for this frame
00003400 monochrome lighting style?
03001600 ffffffff ffffffff ffffffff ffffffff 0f000000 0000
	animation frame 1 key bitmap for full white lighting
03001600 ffffffff ffffffff ffffffff ffffffff 0f000000 0000
	animation frame 2 key bitmap for full white lighting
03001600 ffffffff ffffffff ffffffff ffffffff 0f000000 0000
	animation frame 3 key bitmap for full white lighting
0020 ffffffff ffffffff ffffffff ffffffff 0f000000 0000 RRGGBB00 00003400
	lighting frame key bitmap for full white lighting and monochrome style

So the key flashlight program immediately follows the default light program
without the frame count info?

48040000 03000000 96040000 01000000 looks like it might encode frame count
info for the Nth flashlight program.  03 is the animation frame count.  01 is
the light frame duration.  I don't know how 4804 and 9604 fit in yet.

If I switch to calculator11, this is 2 animation frames, 2 lighting frames.
The 4 ints in question are now 48040000 02000000 7c040000 02000000.  Unclear
how light frame durations of 7 and 2 are encoded.  There is a difference of 52
between 047c and 0448.  Meaningful?  I don't know yet.

Switch to RGB Lights Wave program for key Q.  This has 6 animation frames
duration 2 each.  1 lighting frame duration 20.  Now the 4 ints are 48040000
0c000000 80050000 01000000.  So it looks like 0c is the total animation
duration and 01 is lighting frame count.  Still unclear how the other 2 ints
behave.  Animation frames are individually recorded so duration isn't needed.

0120 bitmap x 22 bytes RRGGBB00 12003400 is lighting frame.  Duration 20, but
how is that coded?  20 is 0x14 but I'm not seeing that.  Style is RGB.  Is
that the 12?  Duration 20 affects the rate of RGB change so it's got to be
here somewhere.

Switch to Windmill1 for key Q.  1 animation frame duration 1.  11 light
frames, each different duration to figure out how it's represented.  4 ints
are 48040000 02000000 62040000 0b000000 with 0b being the 11 light frames.

Light frame durations add up to 385 0x181.  Those numbers aren't showing up
here.  Drat.

With Windmill1 for key Q and no general lighting, the first light program
packet is:

```
0040  27 02 00 00 00 38 2b e1 00 02 00 00 01 00 00 00   '....8+.........
0050  00 00 00 00 00 00 00 00 14 04 00 00 01 00 00 00   ................
0060  2e 04 00 00 0b 00 00 00 ff ff ff ff ff ff ff ff   ................
0070  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff   ................
```

This is different from some other situations.  00020000 starts the packet as
expected.  01000000 indicates 1 animation frame duration 1 as expected.  THe
next 2 ints are 00, which is not expected.  There are then 4 ints
14040000 01000000 2e040000 0b000000.  Here we have 01 and 0b, that seem to
match the animation frame count and light frame count.

This might mean that the two 00 ints indicate no general lighting at all,
which the next 4 indicating lighting program 1 used on the Q key.

I compared two variants of windmill1 by changing the duration of the first
light frame from 60 to 65.  This reminds me that lighting frames look like the
following, also described further down under RGB cycle.

duration 60:
```
0120 keymap RRGGBB00 06003400
```
duration 60:
```
0120 keymap RRGGBB00 05003400
```

They are not identical.  Here we see 0x34 at the end of the frame.  Further
down, we see 0x79 at the end of the frame.

Huh.  When I enable the same windmill1 as the general lighting instead of a
key specific program, I still see 0x34 at the end of the light frames.


## Key code mapping


Key id mapping for driver and custom layer

```
0100   Lctrl
0200   Lshift
0400   Lalt
0800   Windows
1000   Rctrl
2000   Rshift
4000   Ralt
0004   A
0005   B
0006   C
0007   D
0008   E
0009   F
000a   G
000b   H
000c   I
000d   J
000e   K
000f   L
0010   M
0011   N
0012   O
0013   P
0014   Q
0015   R
0016   S
0017   T
0018   U
0019   V
001a   W
001b   X
001c   Y
001d   Z
001e   1
001f   2
0020   3
0021   4
0022   5
0023   6
0024   7
0025   8
0026   9
0027   0
0028   Renter
0029   Esc
002a   Backspace
002b   Tab
002c   Lspace
002d   -_
002e   =+
002f   [{
0030   ]}
0031   \|

0033   ;:
0034   '"
0035   `~
0036   ,<
0037   .>
0038   /?
0039   Capslock
003a   F1
003b   F2
003c   F3
003d   F4
003e   F5
003f   F6
0040   F7
0041   F8
0042   F9
0043   F10
0044   F11
0045   F12
0046   PrtScrn

004b   PageUp
004c   Delete

004e   PageDown
004f   Right
0050   Left
0051   Down
0052   Up

## unsure about these
002a   Middle Backspace	(backspace)
0028   Middle Enter	(right enter)
0100   Middle ctrl	(left ctrl)
2000   Middle shift	(right shift)
002c   Right space	(left space)

## numpad
0053	NumLock
0054	N/
0055	N*
0056	N-
0057	N+
0058	N enter
0059	N1
005a	N2
005b	N3
005c	N4
005d	N5
005e	N6
005f	N7
0060	N8
0061	N9
0062	N0
0063	N.

## media
83010003	media play
cd000003	play/pause
b7000003	stop
b6000003	last song
b5000003	next song
e9000003	vol down
ea000003	vol up
e2000003	mute

## mouse
01000101	left click
02000101	right click
04000101	middle click
08000101	back click
10000101	advance click

## sys/net	these have 03 afterwards - different key layers of some kind?
24020003	browser back
25020003	browser advance
27020003	browser refresh
2a020003	browser collection
23020003	browser home
8a010003	email
94010003	my computer
92010003	calculator
01060002	copy
01190002	paste
0046		screenshot (printscreen)
```


The mouse keys end in 0101.  The sys/net and media keys end in 03.  I think
all of these keys are delivered on the 3rd USB HID interface rather than the
standard keyboard interface.

These were acquired by setting keys with new functions in the custom layer
mapping program.




## macros and Fnx

TODO: I still haven't explored how macros are programmed.  I haven't explored
the Fnx key.

The driver allows unlimited macro sequences in driver mode.  Max 32 char
length in custom layers.

In writing a macro, it appears to record key down events, key up events, and
time in ms between events.

I programmed a macro to type 12345.  The full 19 element sequence is:

```
1 down
112 ms
1 up
94 ms
2 down
82 ms
2 up
78 ms
3 down
73 ms
3 up
120 ms
4 down
95 ms
4 up
101 ms
5 down
79 ms
5 up
```

I then attached it to F1, normally 003a0002.  Now, the F1 spot gets 0000010a.
I assume that 01 is the macro number and 0a indicates macro vs normal key.


Before, there was a fixed sequence between the keymap and lighting program:

```
210204000000 crc 0000 (14 ints)
210205000000 crc 0000 (14 ints)
260200003800 crc ffff (14 ints)
260238003800 crc ffff (14 ints)
260270000800 crc ffff ffff 00 (12 ints)
210206000000 crc 00 x 14
```

Now, we get a more complex sequence that must have the macro encoded:


```
210204000000 crc 0000 (14 ints)
250200003800 crc aa557cfc 13000101
	1e000101 70000003 1e000201 5e000003
	1f000101 52000003 1f000201 4e000003
	20000101 49000003 20000201 78000003
250200003800 crc 21000101 5f000003
	21000201 65000003 22000101 4f000003
	22000201 0000 x 6
250270003800 crc 0000 x 14
2502a8003800 crc 0000 x 14
2502e0003800 crc 0000 x 14
250218013800 crc 0000 x 14
250250013800 crc 0000 x 14
250288013800 crc 0000 x 14
2502c0013800 crc 0000 x 14
2502f8010800 crc 0000 x 14
210205000000 crc 0000 (14 ints)
260200003800 crc ffff (14 ints)
260238003800 crc ffff (14 ints)
260270000800 crc ffff ffff 00 (12 ints)
210206000000 crc 00 x 14
```

I don't know what aa557cfc is yet.  0x13 is 19 decimal, so likely the sequence
length.

	1e000101	1e is the code for 1
	70000003	0x70 is 112 ms
	1e000201    1e is 1.  I assume 02 is key up, with 01 being key down in the
				first code
	5e000003	0x5e is 94 ms
	1f000101    2 key down
	52000003    0x52 is 82 ms
	1f000201    2 key up
	4e000003    78 ms
	20000101	3 key down
	49000003	73 ms
	20000201	3 key up
	78000003	120 ms
	21000101	4 key down
	5f000003	95 ms
	21000201	4 key up
	65000003	101 ms
	22000101	5 key down
	4f000003	79 ms
	22000201	5 key up

I don't know why there are 10 0x25 packets.  Perhaps all macros need to fit
into this space.

Assigning a macro offers a play mode.  There are 3 options:

* Perform N end auto stop

Setting N to 3 changes the start of the following packet:

	250200003800 crc aa557cfc 13000103

Here I now assume 01 is the play mode, and 03 is the count.  In theory, 3
should make the macro play 3 times in a row.  It played twice.

THIS MAY BE A FIRMWARE BUG.  Further testing seems like a driver bug, not
firmware.

* Macro button release stop

This mode gives me the following packet start:

	250200003800 crc aa557cfc 13000203

The play mode is 02.

It keeps playing macro events in a loop while the key is pressed and stops
immediately when the key releases.

Here the 3 matches the N from the count, even though the count doesn't seem to
apply.  This may be a driver program vagary.

* Macro button again press stop

This mode gives me the following packet start:

	250200003800 crc aa557cfc 13000301

The play mode is 03.

When the key is pressed and released, the macro events play in a loop.
Pressing the key again stops the replay.

Q: how do media and mouse keys map?  The key code appears at the first byte of
an int rather than the 2nd.

A: The driver requires you to type the macro program with the keyboard, so you
can't get to the media keys.


Now I add a 2nd macro, with 3 events, i.e. pressing key 1 once.

```
210204000000 crc 0000 (14 ints)
250200003800 crc aa557cfc 13000101
	1e000101 70000003 1e000201 5e000003
	1f000101 52000003 1f000201 4e000003
	20000101 49000003 20000201 78000003
250200003800 crc 21000101 5f000003
	21000201 65000003 22000101 4f000003
	22000201 0000 x 6
250270003800 crc 0000 x 14
2502a8003800 crc 0000 x 14
2502e0003800 crc 0000 x 14
250218013800 crc 0000 x 14
250250013800 crc 0000 x 14
250288013800 crc 0000 x 14
2502c0013800 crc 0000 x 14
2502f8013800 crc 00000000 00000000
	aa551b2d 03010101 1e000101 8b000003
	1e000201 0000 x 7
250230023800 crc 0000 x 14
250268023800 crc 0000 x 14
2502a0023800 crc 0000 x 14
2502d8023800 crc 0000 x 14
250210033800 crc 0000 x 14
250248033800 crc 0000 x 14
250280033800 crc 0000 x 14
2502b8033800 crc 0000 x 14
2502f0031000 crc 0000 x 14
210205000000 crc 0000 (14 ints)
260200003800 crc ffff (14 ints)
260238003800 crc ffff (14 ints)
260270000800 crc ffff ffff 00 (12 ints)
210206000000 crc 00 x 14
```

The first sequence is now 9 0x25 packets long.  The second is 10.  I suspect
that the second sequence is also 9 packets long, and the 10th is a 0
terminator that is there after any number of macros are defined.  This is in
keeping with the fact that I've not found a macro definition count in the
format.

The sequence start has two 0 ints, then aa551b2d.  I don't know what this
means yet.  I suspect the 2 ints may be part of the previous sequence.

OK, the last packet of 0 is partial.  Only 16 valid bytes here.  Then, the
first macro is 9 packets * 56 bytes + 8 bytes = 512 bytes = 128 ints.  The
second macro is 12 * 4 + 56 * 8 + 16 = 512 bytes = 128 ints.

This suggests that each macro can actually be 255 key strokes with delays.

Yes, I can see a longer macro sequence than 32 keys.  My test includes 126
events.

This sequence starts with 

	250200003800 crc aa556469 7e000101

Again, I'm confused by the aa556469.  0x64 is 100.  0x69 is 105.  0x7e is the
macro length.  00 is the macro id.  01 is play mode with N=01.

In this sequence I used left click, left up, right click, and right up.  In
this instance, we have:

	01000102	left click
	01000202	left mouse up
	02000102	right click
	02000202	right mouse up
	
The first 01 and 02 correspond with the mouse button codes.  I assume the 02
indicates mouse vs keyboard.  I'm still wondering if media codes would somehow
work as well and what they would look like.

### CRC for macro codes

aa557cfc 0x7c is 124, 0xfc is 252.
aa551b2d 0x1b is 27, 0x2d is 45.

I still don't know how these codes after aa55 work.  They don't seem to map to
a total duration of the macro.  Or of start and end times.  Or number of
events.

It might be a crc for the macro sequence?

If I change the short macro to be:

2502f8010800 crc 00000000 00000000
	aa5520b3 03010101 00000101 8b000003
	00000201 0000 x 7

the 2 bytes after aa55 are 20b3.

key 2 down, 5ms, key 2 up
2502f8013800 crc 00 x 2
	aa55b0d0 03010101 00000101 05000003
	00000201 00 x 7	

key 2 down, 6ms, key 2 up
2502f8013800 crc 00 x 2
	aa55c518 03010101 00000101 06000003
	00000201 00 x 7	

key 2 down, 7ms, key 2 up
2502f8013800 crc 00 x 2
	aa55165f 03010101 00000101 07000003
	00000201 00 x 7	

Playing with the first macro and changing the repeat count doesn't affect the
aa557cfc.  So those bytes don't appear to be included in the crc-like value.

OK, installed libcrc and played with the bytes and found the answer.  Similar
to the whole packet crc, the aa55 crc is computed with CRC-CCITT-false (or
0xffff) on the bytes in the active steps of the macro.  Trailing 0's are
ignored.

So for 00000101 06000003 00000201, the crc is 0x18c5, matching aa55c518.
For 00000101 07000003 00000201, the crc is 0x5016, matching aa55165f.




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

One piece of information is that a lighting frame applies to a set of active
keys using a bitmap.  The bits are 1 if the key is active, 0 otherwise.

Using the Calculator light program, I get a total sequence as follows:

```
010900000000 crc  00000000 x 14

210301000000 crc  00000000 x 14

220300003800 crc  ffffffff ffffffff
01000002 02000002 04000002 08000002
10000002 20000002 00630002 ffffffff
00040002 00050002 00060002 00070002

220338003800 crc  00080002 00090002
000a0002 000b0002 00600002 005c0002
005d0002 005e0002 00590002 00110002
00610002 00560002 00140002 00150002

220370003800 crc  00160002 00170002
005f0002 00190002 001a0002 001b0002
001c0002 001d0002 001e0002 001f0002
00200002 00210002 00220002 00230002

2203a8003800 crc  00240002 00540002
00550002 00270002 00280002 00290002
002a0002 002b0002 002c0002 002d0002
002e0002 002f0002 00300002 00310002

2203e0003800 crc  00570002 00340002
00350002 005a0002 005b0002 00580002
00390002 003a0002 003b0002 003c0002
003d0002 003e0002 003f0002 00400002

220318013800 crc  00410002 00420002
00430002 00440002 00450002 00460002
ffffffff ffffffff ffffffff ffffffff
004b0002 004c0002 ffffffff 004e0002

220350013800 crc  004f0002 00500002
00510002 00520002 ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

220388013800 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2203c0012000 crc  002a0002 00280002
01000002 20000002 00620002 ffffffff
ffffffff ffffffff 00000000 00000000
00000000 00000000 00000000 00000000

210304000000 crc  00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000

210305000000 crc  00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000

260300003800 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

260338003800 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

260370000800 crc  ffffffff ffffffff
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000

210306000000 crc  00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000

270300000038 crc  00020000 01000000		start of lighting program, 01000000
1a020000 01000000 ffffffff ffffffff		seems to be total animation frame count
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270338000038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270370000038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703a8000038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703e0000038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270318010038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270350010038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270388010038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703c0010038 crc  ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703f8010038 crc  ffffffff ffffffff
03001600 00000000 05006003 00d80000
36008001 00000000 00000020 00000000
05006003 00d80000 36008001 00000000

27033002000a crc  000000ff 00000000
79000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000

0b0300000000 crc  00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
```

0x0109 and 0x2103 seem to always start the custom layer program.  Byte 1 of
0x21 is the layer code 03 for custom layer 2.

Lighting seems to start at command 0x27.  Based on some tests, 

At 2703f801 I believe we have a bitmap of keys with lights enabled for the
pattern.  The specific command number doesn't appear to be fixed.  This is
just 0x01f8 bytes after the start.

03001600 appears to be a prefix for an animation frame.  It is followed by the
animation mask key bitmap.  I think the
0xffffffff ffffffff just has to be there as well.

Q: What are all the ffffffff for?
There are 124 ffffffff here.  This seems to be approximately the number of
keys to program.  Are these direct RGB codes, similar to driver mode
programming?  That seems a lot of bytes to waste on ffffffff.
Perhaps try fixed lighting and see what happens.  Similarly, try the
anim/light frame programming in driver mode.  Does it work?


I copied and modified the Calculator pattern to enable a single light at a
time to try to describe the bitmap.  Treating the first byte after the crc as
byte 0:

```
byte 0 bits 0-7		Esc  F1   F2   F3   F4   F5   F6   ??
byte 1 bits 0-7		F7   F8   F9   F10  F11  F12  Del  ??
byte 2 bits 0-7     Prt  ??   ??   ??   ??   ??   ~    1
byte 3 bits 0-7     2    ??   3    4    5    ??   6    7
byte 4 bits 0-7     8    ??   9    0    -    =    BkSp ??
byte 5 bits 0-7     ??   ??   ??   ??   Tab  Q    W    ??
byte 6 bits 0-7     E    R    T    Xbow Y    U    I    ??
byte 7 bits 0-7     O    P    [    ]    \    PgUp ??   ??
byte 8 bits 0-7     ??   ??   Caps A    S    ??   D    F
byte 9 bits 0-7     G    MBk  H    J    K    ??   L    ;
byte 10 bits 0-7    '    Entr ??   PgDn ??   ??   ??   ??
byte 11 bits 0-7    LShf Z    X    ??   C    V    B    REnt
byte 12 bits 0-7    N    M    ,    ??   .    /    RShf ??
byte 13 bits 0-7    Up   ??   ??   ??   ??   ??   LCtl Win
byte 14 bits 0-7    LAlt ??   ??   LSpc MCtl ??   MShf RSpc
byte 15 bits 0-7    RAlt ??   ??   Fn   RCtl Left Down Rght
```


This is 16 bytes, 4 ints.  After this comes 0x00000000 0x00000020.  I assume
this is a required separator.  The first int might be more keymap bits though.

Based on other info, I think the bitmap is 22 bytes and the separator is
actually 0x0020.

Command 27033002000a contains 10 bytes that are valid (see the 0x0a at the end of
the command).

000000ff 00000000 7900

The 0x79 near the end always seems to show up.  The ff is the green level.  So
in reality we have:

0000RRGG BB000000 7900

I don't yet know what indicates monochrome, RGB cycle, or breathing.


### Experiment on lighting

IMPORTANT: it is possible to force a program on the driver layer.  Using layer
code 0x01, driver layer acts the same as a custom layer.

It seems that this overrides the default backlight programs in driver layer
selectable through Fn F5.


I'm trying this on custom layer 2.

I have 2 frames of keyboard animation:

	Frame 1 Esc
	Frame 2 F1
	
Also 3 frames of lights.

	Frame 1 is Esc, FF00DD (pink), monochrome.
	Frame 2 is F1, 00FFEE (cyan), RBG cycle.
	Frame 3 is All keys, BBAADD (grayish), Breathing.
	
Differing frame counts and colors will hopefully let me see various packet
distinctions.

I'm including only the packets starting with 0x27.  The earlier packets appear
unaffected by lighting changes.

```
270300000038 crc  00020000 02000000		start of lighting program, 01000000
34020000 03000000 ffffffff ffffffff		seems to be total animation frame count
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270338000038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270370000038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703a8000038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703e0000038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270318010038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270350010038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

270388010038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703c0010038 crc  ffffffff ffffffff		never seems to change
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff

2703f8010038 crc  ffffffff ffffffff
03001600 01000000 00000000 00000000
00000000 00000000 00000300 16000200
00000000 00000000 00000000 00000000

27033002000a crc  00000000 00200100
00000000 00000000 00000000 00000000
00000000 ff00dd00 00007900 00200200
00000000 00000000 00000000 00000000

27036802002c crc  00000000 00ffee00
00007900 00207f7f c1dd7d70 7f3fdcdf
0bf777c1 d9f90000 00000000 bbaadd00
00007900 00000000 00000000 00000000

0b0300000000 crc  00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000
```

Packet 270300000038 by ints

	00020000		start of lighting program?
	02000000		Indicates 2 frames of animation
	34020000		The 0x34 is 2 * 0x1a.  Seems matched to animation frame count
	03000000		Indicates 3 frames of lighting
	ffffffff		Remainder of packet.  I haven't seen it change.

Packet 2703f8010038 by int

	ffffffff		2 fixed ints
	03001600		Start of animation key bitmap for 22 bytes
	01000000		Esc is bit 0, byte 0.
	00000000 00000000 00000000 00000000		No other keys set
	00000300		2 bytes of 0.  Then start of 03001600 (key bitmap start)
	16000200		2 bytes of 03001600.  F1 is byte 0, bit 1
	00000000 00000000 00000000 00000000		No keys set

Packet 27033002000a by int

	00000000		Last 4 bytes of key bitmap
	00200100		I think this is 0020 as start of lighting frame.  Then 0100
					is 2 bytes of lighting keymap with Esc set 
	00000000 00000000 00000000 00000000		16 bytes of keymap
	00000000		last 4 bytes of lighting key bitmap
	ff00dd00		RRGGBB00 is color for this frame
	00007900		lighting frame terminator
	01200200		0120 start of lighting frame #2.  01 indicating RGB cycle,
					Then 0200 is F1 in keymap
	00000000 00000000 00000000 00000000		16 bytes of keymap
	
Q: Is 0a correct for packet data size?  This seems wrong.
A: When I compute packet data size consistently, things seem to work.  I'm OK
with leaving as is.


Packet 27036802002c by int

	00000000		last 4 bytes of keymap
	00ffee00		RRGGBB00 color for this frame
	78007900		lighting frame terminator.  Is 78 related to frame length?
	02207f7f		0220 start of lighting frame,  02 is breathing.
					First 2 bytes of keymap is 7f7f
	c1dd7d70 7f3fdcdf 0bf777c1 d9f90000		16 bytes of keymap
	00000000		4 bytes of keymap
	bbaadd00		RRGGBB00
	10000500		frame terminator? 
	00000000 00000000 00000000		unused bytes.  0x2c at end of command
									ignores them

Monochrome frames appear to terminate with 00007900.  I'm seeing an RGB cycle
frame end in 78007900.  Breathing frame ending in 10000500.  However, the
leading byte of the frame also has a different code. 

The monochrome frame is duration 2.  The RGB frame is duration 3.  The
breathing frame is 6+5.

I don't understand the frame terminator.

Dropping the breathing frame, I have a monochrome frame duration 4, RGB frame
duration 3.  I repeat the capture with monochrome frame duration 7, RGB frame
duration 2.

Packet 27026802000c		duration 4/3

	78 00 79 00			01111000 00 79 00
	
Packet 27026802000c		duration 7/2

	b4 00 79 00			10110100 00 79 00
	
Clues but no answer yet.

### Simplifying things

1 frame animation, duration 20, Esc selected.
1 frame lighting, duration 1, Esc selected, Red FF0000, monochrome

```
270300000038 crc  00020000 14000000		start of lighting program, 01000000
08040000 01000000 ffffffff ffffffff		seems to be total animation frame count
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
```

0x14 is 20.  This is the animation frame duration.

Now 0804.  I normally see 1a02 when the animation is duration 1.  Here, if I
first subtract 02 from 04, I have 0802.  Treating this as a little endian
short, I have 0x0208.  0x0208 / 20 gives 0x1a, which is the number I see in
duration 1 animations.

So this suggests that those 2 bytes are 0x0200 + 0x1a * duration.
The 0x0200 is very specific.  Changing it makes the lighting effect fall back
to white.

IMPORTANT: 0x1a is 26.  I *think* this is 4 bytes bitmap leader and 22 bytes
of bitmap.

I next see 20 copies of 03001600 followed by a 22 byte keymap.  After that is:

	0020	 followed by 22 byte keymap
	RRGGBB00
	00007900
	
Now let's reverse this.  1 duration animation, 20 duration lighting.

```
270300000038 crc  00020000 01000000		start of lighting program 1 anim frame
1a020000 01000000 ffffffff ffffffff		There's 01 here, where I was expecting 0x14
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
```

I can't see the effect.  Changing to 5 duration lighting has no difference with
20 duration lighting.  I think the animation duration has to exceed the lighting
duration.

Trying duration 20 animation, duration 20.  This is no different than duration
20 animation, duration 1 lighting.  Perhaps the driver just figures that
there's no need to do anything different.

### Trying RGB cycle durations

Windmill has 1 frame animation, 11 frames of lighting, each duration 60.  I
tried changing the durations from 60 down to 10 by 5's.  

```
270300000038 crc  00020000 01000000		start of lighting program 1 anim frame
1a020000 0b000000 ffffffff ffffffff		The 0x0b seems to number of light frames
ffffffff ffffffff ffffffff ffffffff
ffffffff ffffffff ffffffff ffffffff
```

0x0b is 11, which is the number of light frames, not the durations.

In the lighting frames, I see the RGB0 followed by a number before the 0x79
frame terminator.

The 11 sequences are:

0120 keymap RRGGBB00 06007900
0120 keymap RRGGBB00 06007900
0120 keymap RRGGBB00 07007900
0120 keymap RRGGBB00 08007900
0120 keymap RRGGBB00 09007900
0120 keymap RRGGBB00 0a007900
0120 keymap RRGGBB00 0c007900
0120 keymap RRGGBB00 0e007900
0120 keymap RRGGBB00 12007900
0120 keymap RRGGBB00 18007900
0120 keymap RRGGBB00 24007900

The leading 0120 indicates RGB cycle style.

The first byte of the 79 int looks like an inverse of the duration.  We have

	byte  duration
	6		60
	6		55
	7		50
	8		45
	9		40
	10		35
	12		30
	14		25
	18		20
	24		15
	36		10

The duration is the inverse of the byte value.  So x/60 = 6 gives x=360.  This
works for 36,10.  In general, it looks like:

	byte = floor(360/duration)


### Trying breathing patterns

Breathing cycles dim and brighten the light as well as shift the RGB color
over time.

Breathing patterns have 2 numbers for the duration: breathing change, and
breathing stop.  The first number controls how fast or slow the brightness
varies up and down.  The second controls gaps between patterns.

Using a copy of Flow, I changed the RGB patterns to breathing patterns.  The
animation is 22 cycles, 22 frames.

0220 keymap RRGGBB00 05000100
0220 keymap RRGGBB00 0a000200

The leading 0220 indicates breathing style.

The first frame was duration 20+1.  The second was 10+2.  The second is
obvious how it applies.  The first is not.

Change 20+1 to 1+1.

0220 keymap RRGGBB00 64000100
0220 keymap RRGGBB00 0a000200

There appears to be an inverse relationship here again, at least with the
breathing change.  Breathing stop appears to be exactly the number entered.
Breathing change relationship:

	byte	duration
	16		10
	5		20
	100		1
	50		2
	33		3
	25		4
	1		100
	
	byte = floor(100/duration)

Q: since mono is 00 duration and 0x79 stop, is there a consistent pattern to
how these numbers are specified?




### How do animation and lighting interact?

I've thought that the animation and lighting keymaps are AND'd to arrive at
the enabled LEDs, but I'm not sure about this.

After exploring the Colorful Rainbow pattern, I think a monochrome light frame
is always on.  If two frames give the same key a light, I believe the latter
frame takes precedence over the former.

With this in mind, then, lighting frames are always running.  A monochrome
frame lights up the specified keys with the chosen color.  Duration shouldn't
matter.  An RGB frame cycles through colors starting with the specified one.
Duration specifies the number of cycles it takes to return to the starting
color.  A breathing frame has 2 durations - breathing change and breathing
stop.  I don't know how these work yet.  But all 3 pattern types continuously
cycle.

Animation frames are masks.  When an animation frame has a key selected, any
color patterns are allowed through.

Windmill works by having each light frame use the RGB pattern starting on a
different color.  Each successive frame is set so that the color change
happens in order.  In other words, frame 2 will hit full red 5 cycles after
frame 1, frame 3 hits full red 5 cycles after frame 2, and so on.

Next, each frame maps to a slice of a wheel, so that the color peaks will
happen in successive slices, giving the illusion of a turning.  In this case,
the animation frame just allows everything through.

### Animation frames

Animation frames are a mask that allows the lighting program to show through.
Individual frames do not have a duration.  The windows driver implements
duration by repeating the frame.  So if you have 2 animations frames, with
frame 1 having duration 2, and frame 2 having duration 1, the program sent to
the keyboard contains 2 copies of frame 1 and 1 copy of frame 2.

The first lighting packet contains 0x1a * frame count + 0x200.  I suspect 0x1a
tunes the overall frame duration.

Unfortunately, tweaking this value causes the lighting to revert to white.  So
there doesn't seem to be flexibility here to play with durations.


### Simulating driver style lighting program

Q: can we simulate simple frame lighting?

I don't know if this is actually doable.  The lighting frames don't easily
allow specifying a key position as a fixed color, then changing it.  Perhaps
breathing can be adapted to do the right thing.

Early on, the 0x27 lighting program section has 120 0xff values.  This is the
right size to contain an RGB frame.  Initial tests suggest it doesn't work,
but there may be something else that needs to change.
