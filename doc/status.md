# Status of features in the Linux driver

## Common features

* yaml cfg to set up keyboard
* yaml include file support
* API to control keyboard
* example cfgs
* access keyboard version
* specify colors by name or rgb code
* parse keymaps
* parse macros
* parse custom lighting
* parse flashlight cfg
* parse light sequence (driver mode)
* library of light programs




### NOT IMPLEMENTED YET

* flashing firmware
(more to revengineer)
* handle more than one xbows keyboard simultaneously (or distinguish them)
(more to revengineer)
* config library of flashlights (that doesn't require driver prog - I *think*
  this can be done)
(programming)
* gui program to wrap xbows system
(programming)
* good docs for programming the keyboard
(writing)


## API

* switch between layers
kbdtest2.cc -l layer
	2,3,4 custom layers  5 driver (temporary)
	1 switches to default static layer


### NOT IMPLEMENTED YET
* factory reset
(programming)
* good examples of programmatic control
perhaps assigning calc to driver mode
(writing)

## Driver layer programming

### Implemented

* keep driver program alive
* lighting as sequence of colormaps
* keymap assignment


### NOT IMPLEMENTED YET

* Restore driver program after using FnX to switch layers
(programming)
* macros for driver layer
(more to revengineer)
* flashlights for driver layer
(more to revengineer)

## Custom layer programming

### Implemented

* lighting program (anim and color frames)
* keymap
* macros
* flashlights
* disable keys
* FnX support


### NOT IMPLEMENTED YET

* feed empty light program if only a flashlight is provided?
no easy way to recall the existing lighting program.
else just require a lighting program
(design)

* Full set of lighting programs from windows driver
(programming)
* lighting sequence?
This might be doable by continually writing the strange lighting program.
However, this may clobber the flash mem.

Try again and see if we can feed similar commands to cust layer as we do to
driver layer.

(more to revengineer)
