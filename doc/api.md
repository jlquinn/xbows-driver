# XBows library API

The program struct is the data structure that you will build to program the
XBows keyboard.

XBows supports two programming modes: driver and custom.  The driver layer
supports temporary programming, with the keyboard reverting to its defaults
after several seconds.  Custom layer changes are permanent until altered.

## Important data structures

keycodes
	This is an enum mapping an identifier for each key to a number.  Keycodes
	are used in various APIs.
	


## Driver mode programming

Driver mode supports two features: a light program and a key remapping.
Either or both can be specified.

The key mapping is stored in program.kmap.  This holds the mappings for each
key.  When you construct a keymap, it is initialized with the default
mappings.

	kmap.assign(key, emits)
		Program key (specified by a keycode) to emit emits (also a keycode).
		
Call assign() for each key you wish to remap.

Driver mode doesn't directly support macros.  These must be emulated in
software.


Driver mode light programs are created by giving a sequence of frames.  Each
frame assigns RGB values to each key.  When a frame is sent to the keyboard,
the lights will change to match the frame.

A light program is implemented by sending frames in sequence to the keyboard.
The software must keep sending frames to the keyboard to maintain the light
program.

Driver mode doesn't directly support flashlights.  These must be implemented
by sending sequences of frames to the keyboard as well.

Driver mode frames are assembled into program.lights.  This structure is an
rgb_lights, which is a vector of rgb_frame.  An rgb_frame contains an RGB
value for each key of the keyboard.  By default, the value is 0.

To set an rgb value for a key:

	rgb_frame.setkey(key, RGB)
		where key is the keycodes of the key to modify.  RGB stores the RGB
		value with R in the least significant byte 0, G in byte 1, B in
		byte 2.  The top byte must be 0.

In addition, a frame may contain a delay field.  This tells the library to
wait this many milliseconds before sending the frame to the keyboard.  The
default value is 0.  This provides a simple means of changing the duration of
each frame in a light program.

program.layer must be 0 for driver mode.


## Custom layer program

The XBows keyboard supports custom layers that remember their programing.
There are 3 custom layers, each of which can support 4 types of programming.

Key remapping

Light program

Macros

Flashlights

Custom layer key remapping is identical to driver mode key remapping.  In
fact, the same datastructure is used: program.kmap.

Custom layer light programs are different.  A custom light program is stored
in program.custom_lights.  A custom light program is constructed from 2 parts:
animation frames, and light pattern frames.

Light pattern frames describe how keys are assigned colors.  Each key can be
assigned one light pattern.  These patterns do not change throughout the light
program.  However, some patterns cause the key to change color over time.

The three light patterns supported are monochrome, RGB cycle, and breathing.
The monochrome light pattern gives a fixed color to a key.  RGB cycle starts
with the key on a given color.  The color shifts to other colors before
eventually returning to the original color.  Breathing starts a key with a
color, which then dims to black, and returns to full brightness.  At the same
time, the color changes over time, eventually returning to the original color.

Animation frames add the ability to control whether a key's light pattern is
visible or not.  Animation frames are a mask that exposes the light pattern
for some keys and masks others.  Masked keys are black.  Exposed keys show the
underlying pattern.  Animation frames are played in sequence in a loop,
allowing very dynamic light programs when combined with light patterns.

Light pattern frames are all active at the same time.  If multiple light
patterns are assigned to a key, only one will be used.  Animation frames are
played in sequence, one after the next.


As mentioned above, an animation frame is just a mask.  By default, all keys
are masked.  To expose a key, call

	animation_frame.enable(key)
		Cause key to expose a light pattern if assigned.
		
To construct an animation frame, create the object, then call enable with each
key you want exposed for that frame.  Add the frame to
program.custom_lights.aframes.  Keep adding animation frames until the program
is complete.

To construct a light pattern frame, create a pattern_frame. Choose the keys to
apply this pattern to, using the pattern_frame::enable(key) function.  Set the
pattern using one of:

	pattern_frame::monochrome(red, green, blue)
	pattern_frame::rgb_cycle(red, green, blue, duration)
	pattern_frame::breathe(red, green, blue,  duration, gap)
	
In all 3 functions, red, green, and blue specify the starting color.  Each
value is [0,255].  For rgb_cycle, duration gives the length of the color
shifting cycle.  For breathe, duration controls how long the dim/brighten
sequence takes.  The gap value specifies the number of frames the brightness
change pauses between cycles.


Macro programming - XXX NOT IMPLEMENTED YET

Flashlights are light programs that are activated when a key is pressed.  Each
key may have a flashlight attached to it.  A flashlight program is specified
the same way as the custom light program.  The list of flashlights goes into
program.flashlights.  To assign a flashlight to indivual keys, put the index+1
of the flashlight into program.flashlight_keys[key] where key is the keycode
to assign the flashlight to.  A particular flashlight program may be assigned
to multiple keys.

program.layer must be 1, 2, or 3 corresponding to the layer to modify.


## Programming the keyboard

First call init_kbows() to create an xbows_state.  Once you have a program,
you can send it to the keyboard.  Call

	bool xbows_send(&state, program)

to send the program to the keyboard.

If you are sending a driver mode program, it will send the keymap and the
first light program frame and return true.  Repeatedly calling
xbows_update(&state) will send successive light frames to the keyboard and
keep the driver mode program alive.  If a light frame has a delay, it will
wait this long before sending the frame.  If you wait too long, the driver
layer will revert to its default.

If you are sending a custom mode program, xbows_send() will return false.

Call xbows_switch_layer() if you wish to change layers from software.  Call
xbows_factory_reset() to restore the default programs to the keyboard.  XXX
NOT IMPLEMENTED YET.

When done, call xbows_close() to close the underlying handles.

Usually you won't need to look inside xbows_state.  But keyboard info is
stored in a number of strings there.

	
