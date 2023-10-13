// Revised test program to talk to xbows keyboard

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include <hidapi/hidapi.h>

#include "custom_layer.hh"
#include "driver_layer.hh"
#include "driver_parse.hh"
#include "timer.hh"
#include "xbows.hh"

using namespace std;

void usage(char* prog) {
    cerr << prog << "[options] <cfg.yaml>\n";
    cerr << "-n    Don't sent programming packets to the keyboard\n";
    cerr << "-v    Print verbose messages\n";
    cerr << "-r    Reset XBows keyboard\n";
    cerr << "-l n  Switch XBows to layer n (2,3,4,5) (ignores config file)\n";
    cerr << "-c    Continuous - keep sending idle packets in driver mode\n";
    exit(-1);
}

int main(int ac, char* av[]) {

  xbows_state state = init_xbows();


  //// Send a basic driver mode light program ////
  // TODO
  // Convert a program to a sequence of packets
  // Send the packet sequence to the keyboard.
  // Each program is going to be somewhat different.
  // I need to represent a program, and the packet sequence.

  // // experiment with layer switching
  // vector<packet> prog;
  // prog.push_back(packet(0x0b, 0x05));
  // prog.back().compute_crc();
  // send_program(dev, prog);

  
  // Let's try sending the green calculator program to the keyboard in driver
  // mode



  bool dryrun = false;
  bool continuous = false;
  
  int c;
  while ((c = getopt(ac, av, "nvrl:c")) != -1) {
    switch (c) {
    case 'n': dryrun = true; break;
    case 'r':
      xbows_factory_reset(&state);
      cerr << "Sent reset to XBows\n";
      exit(0);
    case 'v': break;
    case 'l':
      xbows_switch_layer(&state, layercode(atoi(optarg)));
      exit(0);
    case 'c': continuous = true; break;
    default:
      usage(av[0]);
    }
  }
  if (optind == ac)
    usage(av[0]);
  
  state.suppress = dryrun;

  ifstream ifs(av[optind]);
  program prog = read_config(ifs);

  // Autoselect layer?
  bool more = xbows_send(&state, prog);
  if (continuous) more = true;

  while (more) {
    more = xbows_update(&state);
    if (continuous) more = true;
    // sleep some
    // break eventually
    // break;
  }

  // prog.rgb_lights = make_calc();
  // prog.rgb_lights = make_trail();
  
  // bool more = xbows_send(state, prog);
  // while (more) {
  //   xbows_update(state);
  //   // sleep some
  //   // break eventually
  // }


  // prog.kmap.assign(K_Z, K_Q);

  // bool more = xbows_send(state, prog);
  // while (more) {
  //   xbows_update(state);
  //   // sleep some
  //   // break eventually
  // }
  
  
  // prog.clear();
  // prog.custom_lights.aframes.push_back(aframe1);

  
  // // animation_frame aframe1;
  // // aframe1.enable(K_MEnter);
  // // aframe1.enable(K_Esc);
  // // lightprog.aframes.push_back(aframe1);
  
  // // // cus_anim_frame aframe2;
  // // // enable_key(aframe2.keymap, K_Esc);
  // // // lightprog.aframes.push_back(aframe2);
  // prog.custom_lights.aframes.push_back(aframe2);

  // // pattern_frame lframe1;
  // // lframe1.enable(K_Esc);
  // // lframe1.enable(K_MEnter);
  // // // lframe1.monochrome(0xff, 0, 0);
  // // // lframe1.rgb_cycle(0xff, 0, 0, 100);
  // // lframe1.breathe(0, 0xff, 0, 30, 5);
  // // lightprog.lframes.push_back(lframe1);
  // prog.custom_lights.aframes.push_back(lframe1);
  // bool more = xbows_send(state, prog);


  xbows_close(state);
  
  return 0;
}
