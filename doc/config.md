Docs on linux driver configuration


# Defining a keyboard configuration

## Defining mapping for a key

	keyname = newval

For example

	CapsLock = Control_L
	
will tell the CapsLock key to return Left Control to the computer.

## RGB mapping

	keyname = #RRGGBB
	keyname = colorname
	
Both forms are valid ways of specifying an RGB color.  #RRGGBB is the hex code
for the color.  The value is case insensitive.  For example:

	F1 = #ff00EE
	
assigns F1 a purple color.  Keyname can be a symbol, so you might have:

	= = white
	
This assigned white to the = key.

## Macro specification

Macros are a combination of events, one of key down, key up, or delays.  The
specification of an event looks like the following:

	Control_L down
	10 ms
	X down
	5 ms
	X up
	20 ms
	Control_L up

A key up or down event is keyname followed by "down" or "up".  A delay is an
integer followed by "ms" for milliseconds.


## Defining layers

### Driver layer

