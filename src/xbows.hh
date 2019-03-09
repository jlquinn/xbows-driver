#ifndef XBOWS_HH
#define XBOWS_HH

#include "driver_layer.hh"
#include "custom_layer.hh"

void init_xbows();

// Combination of custom layer program and driver layer programming.
struct program {
  vector<drv_light_frame> lights;
  drv_keymap kmap;		// same as cus_keymap
  custom_light_prog custom_lights;
};

#endif
