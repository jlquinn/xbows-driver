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
	  - { keyname: color, keyname: color ... }
	  - delay
	  - { keyname: color, keyname: color ... }
	  
```

This is a sequence of rgb frames and optional delays.  The delay is an integer
number of milliseconds before the next frame is displayed.  The default is 0.

Each frame is a map of keyname to color.  Keyname strings are the same as for
keymaps.  Color can be either a color name or an RR GG BB hex code sequence.
Both forms are valid ways of specifying an RGB color.  Both hex codes and
color names are case insensitive.  For example:

For example
```
	colormap:
	  - {F1: [ff,00,EE], Equal: white}
```
	
assigns F1 a purple color.  The Equal key is assigned a white color.

## Macro specification

Macros are a combination of events, one of key down, key up, or delays.  The
specification of an event looks like the following:

```yaml
macros:
  - id: 1
    mode: once/release/loop    # default to once
    steps:
      - Control_L down
      - 10 ms
      - X down
      - 5 ms
      - X up
      - 6 ms
      - Control_L up
    
  - id: 2		# sequence of steps for the macro
    steps: [ K down, 5 ms, K up, 6 ms ]
```

A key up or down event is the keyname followed by 'down' or 'up'.  A delay is an
integer in milliseconds followed by 'ms'.


## Defining layers

### Driver layer

### Custom layer

Define yaml file for doing this stuff.  Much easier than hardwired parsing

We have maps of key to color, key to key, key to macro, key to flashlight.

# Yaml config file syntax

```yaml
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
    C: { macro: 1 }		# assign macro 1 to C key

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
    C: { macro: 1 }		# assign macro 1 to C key
  macros:
  - id: 1
    mode: once/release/loop    # default to once
    steps: [ Q down, 5 ms, Q up, 6 ms]
    
  - id: 2		# sequence of steps for the macro
    steps: [ K down, 5 ms, K up, 6 ms ]
  
  flashlight:
    # same as light program  
```

# Including from the shared library

An include mechanism is supported by specifying a filename with the tag !include.
For example:

```
lights: !include effect-rgb-light-wave.yaml
```

This will load the file effect-white.yaml and insert the contents
under the lights node.  In effect-white.yaml, you might have

```yaml
animation:  # list of frames
- all
- all
- all
pattern:
- color: [ff,ff,ff]
  type: monochrome
  duration: 1
```

where all the content under the lights map label is present in that file.

In yaml terms, a scalar can be replaced by the contents of another file.  This
provides a means of inserting a complex structure into the structure of the
current file.

Simple substitution is supported.  Each file is loaded separately so must be a
valid yaml file by itself.
