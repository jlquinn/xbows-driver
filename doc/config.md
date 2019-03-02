Docs on linux driver configuration


# Defining a keyboard configuration

## Defining mapping for a key

Keymaps are defined as follows:

```
	keymap:
	  keyname: emits
	  keyname: emits
	  ...
```


For example
```
	keymap:
	  CapsLock: Control_L
```

will tell the CapsLock key to return Left Control to the computer.

## RGB mapping

Colormaps are defined as follows:

```
	colormap:
	  keyname: #RRGGBB
	  keyname: colorname
	  ...
```

Both forms are valid ways of specifying an RGB color.  #RRGGBB is the hex code
for the color.  Both hex codes and color names are case insensitive.  For example:

For example
```
	colormap:
	  F1: #ff00EE
```
	
assigns F1 a purple color.  Keyname can be a symbol, so you might have:

	Equal: white
	
This assigned white to the = key.

## Macro specification

Macros are a combination of events, one of key down, key up, or delays.  The
specification of an event looks like the following:

```
  macros:
  - id: 1
    mode: once/release/loop    # default to once
	steps:
	  - down: Control_L
	  - delay: 10
	  - down: X
	  - delay: 5
	  - up: X
	  - delay: 6
	  - up: Control_L
	
  - id: 2		# sequence of steps for the macro
	steps: [ down: K, delay: 5, up: K, delay: 6 ]
```

A key up or down event is down or up followed by keyname.  A delay is an
integer in milliseconds.


## Defining layers

### Driver layer

### Custom layer

Define yaml file for doing this stuff.  Much easier than hardwired parsing

We have maps of key to color, key to key, key to macro, key to flashlight.

# Yaml config file syntax


driver:
  # defines light program.  Just a sequence of color frames
  lights:
  - frame:
    colormap:
	  {8: green, 9: green, U: green, I: green, O: green, P: green,
	  J: green, K: green, L: green, Semicolon: green,
	  M: green, Comma: green, Period: green, Slash: green,
	  Space_R: green, Alt_R: green }
  - frame: keymap
  
  
  # define key mapping
  keymap:
	Z: Q            # assign Q to the Z key
	C: macro: 1		# assign macro 1 to C key
	
  macros:
  - id: 1
	steps: []	
  - id: 2
	steps: []	
	  
  flashlight:
    - id: 1			# unique id for the flashlight program
	  - colormap: {...}
	  - colormap: {...}
	
	
---

custom:
  layer: 1    # optional to choose the custom layer
  lights:
   animation:  # list of frames
   - [Q, K, Space]
   - [M, N, O]
   lighting:   # list of lighting commands
   - color: RGB/name
	 keys: [Q, K, Space]   # list of active keys
	 pattern: monochrome/cycle/breathe
	 duration: # only for breathe and cycle
	 gap: 5    # only used for breathe
   COLORMAP: ????  # Is it possible to specify custom as a sequence of colormaps?
	
  keymap:
	Z: Q            # assign Q to the Z key
	C: macro: 1		# assign macro 1 to C key
  macros:
  - id: 1
    mode: once/release/loop    # default to once
	steps:
	  - down: Q
	  - delay: 5
	  - up: Q
	  - delay: 6
	
  - id: 2		# sequence of steps for the macro
	steps: [ down: K, delay: 5, up: K, delay: 6 ]
  
  flashlight:
	# same as light program  
