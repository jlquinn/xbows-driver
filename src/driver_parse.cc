// This code is to parse a configuration for driver mode.  It is different
// from custom mode unfortunately.

#include <cctype>
#include <stdexcept>
#include <istream>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <yaml-cpp/yaml.h>

#include "keymap.hh"
#include "xbows.hh"

using namespace std;
using namespace boost::algorithm;

string lower(const string& s) {
  // lowercase the name
  string v = s;
  for (auto& c : v)
    c = tolower(c);
  return v;
}


void dump(YAML::Node node) {
  switch (node.Type()) {
  case YAML::NodeType::Null: // ...
    cout << "node null" << endl;
    break;
  case YAML::NodeType::Scalar: // ...
    cout << "node scalar " << node.Scalar() << endl;
    break;
  case YAML::NodeType::Sequence: // ...
    cout << "node seq len " << node.size() << endl;
    for (unsigned i=0; i < node.size(); i++)
      dump(node[i]);
    break;
  case YAML::NodeType::Map: // ...
    cout << "node map " << node.size() << endl;
    for (auto it: node) {
      cout << " key "; dump(it.first); cout << endl;
      cout << " val "; dump(it.second); cout << endl;
    }
    break;
  case YAML::NodeType::Undefined: // ...
    cout << "node undefined " << endl;
    break;
  }
}



unordered_map<string, keycodes> namemap;

void init_namemap() {
  // Letters
  namemap["A"] = K_A;
  namemap["B"] = K_B;
  namemap["C"] = K_C;
  namemap["D"] = K_D;
  namemap["E"] = K_E;
  namemap["F"] = K_F;
  namemap["G"] = K_G;
  namemap["H"] = K_H;
  namemap["I"] = K_I;
  namemap["J"] = K_J;
  namemap["K"] = K_K;
  namemap["L"] = K_L;
  namemap["M"] = K_M;
  namemap["N"] = K_N;
  namemap["O"] = K_O;
  namemap["P"] = K_P;
  namemap["Q"] = K_Q;
  namemap["R"] = K_R;
  namemap["S"] = K_S;
  namemap["T"] = K_T;
  namemap["U"] = K_U;
  namemap["V"] = K_V;
  namemap["W"] = K_W;
  namemap["X"] = K_X;
  namemap["Y"] = K_Y;
  namemap["Z"] = K_Z;

  // Numbers
  namemap["1"] = K_1;
  namemap["2"] = K_2;
  namemap["3"] = K_3;
  namemap["4"] = K_4;
  namemap["5"] = K_5;
  namemap["6"] = K_6;
  namemap["7"] = K_7;
  namemap["8"] = K_8;
  namemap["9"] = K_9;
  namemap["0"] = K_0;

  // Other printing chars
  namemap["Minus"] = K_Dash;
  namemap["Equal"] = K_Equal;
  namemap["LeftBracket"] = K_LBracket;
  namemap["RightBracket"] = K_RBracket;
  namemap["BackSlash"] = K_BkSlash;
  namemap["Semicolon"] = K_Semi;
  namemap["Quote"] = K_Quote;
  namemap["Comma"] = K_Comma;
  namemap["Period"] = K_Period;
  namemap["Slash"] = K_Slash;
  namemap["BackQuote"] = K_Backquote;

  // Nonprinting keys
  namemap["Escape"] = K_Esc;
  namemap["Tab"] = K_Tab;
  namemap["CapsLock"] = K_Capslock;

  namemap["F1"] = K_F1;
  namemap["F2"] = K_F2;
  namemap["F3"] = K_F3;
  namemap["F4"] = K_F4;
  namemap["F5"] = K_F5;
  namemap["F6"] = K_F6;
  namemap["F7"] = K_F7;
  namemap["F8"] = K_F8;
  namemap["F9"] = K_F9;
  namemap["F10"] = K_F10;
  namemap["F11"] = K_F11;
  namemap["F12"] = K_F12;

  namemap["Enter_R"] = K_REnter;
  namemap["Enter_M"] = K_MEnter;

  namemap["Shift_L"] = K_LShft;
  namemap["Shift_M"] = K_MShft;
  namemap["Shift_R"] = K_RShft;

  namemap["Control_L"] = K_LCtrl;
  namemap["Control_M"] = K_MCtrl;
  namemap["Control_R"] = K_RCtrl;

  namemap["Alt_L"] = K_LAlt;
  namemap["Alt_R"] = K_RAlt;

  namemap["BackSpace_M"] = K_MBksp;
  namemap["BackSpace_R"] = K_RBksp;

  namemap["Space_L"] = K_LSpc;
  namemap["Space_R"] = K_RSpc;

  namemap["Windows"] = K_Win;

  namemap["PageUp"] = K_PgUp;
  namemap["PageDown"] = K_PgDn;

  namemap["Left"] = K_Left;
  namemap["Right"] = K_Right;
  namemap["Up"] = K_Up;
  namemap["Down"] = K_Down;

  namemap["PrintScreen"] = K_PrtScrn;
  namemap["Delete"] = K_Del;

  namemap["XBows"] = K_Xbows;
			// Light only
  // Numpad
  namemap["NumLock"] = K_Numlock;
  namemap["NP_Asterisk"] = K_NSlash;
  namemap["NP_Star"] = K_NStar;
  namemap["NP_Enter"] = K_NEnter;

  namemap["NP_1"] = K_N1;
  namemap["NP_2"] = K_N2;
  namemap["NP_3"] = K_N3;
  namemap["NP_4"] = K_N4;
  namemap["NP_5"] = K_N5;
  namemap["NP_6"] = K_N6;
  namemap["NP_7"] = K_N7;
  namemap["NP_8"] = K_N8;
  namemap["NP_9"] = K_N9;
  namemap["NP_0"] = K_N0;

  namemap["NP_Period"] = K_NPeriod;
  namemap["NP_Minus"] = K_NMinus;
  namemap["NP_Plus"] = K_NPlus;
 
  // Media
  namemap["MediaPlay"] = K_MediaPlay;
  namemap["MediaPause"] = K_MediaPause;
  namemap["MediaStop"] = K_MediaStop;
  namemap["MediaLast"] = K_MediaLast;
  namemap["MediaNext"] = K_MediaNext;

  namemap["MediaVolUp"] = K_MediaVolUp;
  namemap["MediaVolDown"] = K_MediaVolDown;
  namemap["MediaMute"] = K_MediaMute;

  // Mouse
  namemap["Mouse_L"] = K_LClick;
  namemap["Mouse_M"] = K_MClick;
  namemap["Mouse_R"] = K_RClick;
  namemap["Mouse_Back"] = K_BackClick;
  namemap["Mouse_Forward"] = K_FwdClick;

  // Sys/net
  namemap["NetBack"] = K_NetBack;
  namemap["NetForward"] = K_NetFwd;
  namemap["NetRefresh"] = K_NetRefresh;
  namemap["NetCollection"] = K_NetCollection;
  namemap["NetHome"] = K_NetHome;

  namemap["NetEmail"] = K_NetEmail;
  namemap["NetComp"] = K_NetComp;
  namemap["NetCalc"] = K_NetCalc;
  namemap["NetCopy"] = K_NetCopy;
  namemap["NetPaste"] = K_NetPaste;
  namemap["NetPrtScrn"] = K_NetPrtScrn;

  namemap["Home"] = K_Home;
  namemap["End"] = K_End;
  namemap["Insert"] = K_Ins;

  // Not on xbows kbd
  namemap["Fn"] = K_Fn;
}


// Convert a keyname to the corresponding keycode, returning K_NONE if it's
// not present.
keycodes string_to_key(const string& keyname) {
  auto search = namemap.find(keyname);

  // Handle bad keynames
  if (search == namemap.end())
    return K_NONE;
  
  return search->second;
}

struct rgb {
  int R, G, B;
  operator uint32_t() const { return R + (G << 8) + (B << 16); }
};
unordered_map<string, rgb> colornames;

void load_colornames() {
  // XXX this is on Ubuntu 18.  Other systems may be different.  This needs to
  // be more sophisticated.
  string filename = "/usr/share/X11/rgb.txt";
  ifstream ins(filename);
  if (!ins)
    throw runtime_error(string("Failed to load color names from ") + filename);
  string line;
  while (getline(ins, line)) {
    // Skip non-colorname lines
    if (line.empty() || line[0]=='!')
      continue;
    // num num num name ...
    vector<string> bits;
    split(bits, line, is_space(), token_compress_on);
    if (bits.size() < 4)
      throw runtime_error("Bad line format");
    // lines that have leading spaces create an extra entry up front
    if (bits[0].empty()) bits.erase(bits.begin());
    // Check first 3 are numbers
    for (int i=0; i < 2; i++)
      if (!all(bits[i], is_digit()))
	throw runtime_error("Bad line format");
    int R = stoi(bits[0]);
    int G = stoi(bits[1]);
    int B = stoi(bits[2]);
    bits.erase(bits.begin(), bits.begin()+3);
    string name = join(bits, " ");
    // lowercase the name
    name = lower(name);
    // cout << "colorname name " << name << endl;

    colornames[name] = {R, G, B};
  }
}

void bad_color(const string& color) {
  string msg = "Illegal color name specified: " + color;
  throw runtime_error(msg);
}

rgb string_to_color(const string& color) {
  // Look up color name
  auto rgbiter = colornames.find(lower(color));
  if (rgbiter == colornames.end())
    bad_color(color);
  rgb RGB = rgbiter->second;
  return RGB;
}

rgb node_to_color(YAML::Node node) {
  if (node.IsSequence()) {
    // dump(node);
    if (node.size() != 3)
      throw runtime_error("RGB color sequence must be 3 values ");
    rgb RGB = {
      stoi(node[0].as<string>(), 0, 16),
      stoi(node[1].as<string>(), 0, 16),
      stoi(node[2].as<string>(), 0, 16),
    };
    // cout << "read rgb " << hex << RGB << endl;
    return RGB;
  }
  // Color name
  return string_to_color(node.as<string>());
}



// Define a format
// Light frame is [key rgb ... key rgb]
// light program is sequence of frames, and gap between them
//
// keymap is [key assignment ... key assignment]
//
// macro is more complex

// allow color names?  If so, where to get them from?  Can I leverage X color
// tables?


void parse_driver_cfg(YAML::Node node) {
  if (node["lights"]) {
    YAML::Node lt = node["lights"];
    // read a light program
    if (lt.Type() != YAML::NodeType::Sequence) {
      throw runtime_error("not sequence");
    }
    for (auto frame: lt) {
      // int id = frame["id"].as<int>();
      auto steps = frame["steps"];
      for (auto step: steps) {
	// process each macro step
	if (step["down"]) {}
	if (step["up"]) {}
	if (step["delay"]) {}
      }

    }
    
  }
  if (node["keymap"]) {
    // read a keymapping
  }

  if (node["macros"]) {
    // read macros
  }
  if (node["flashlight"]) {
    // read flashlight programs
  }
}

// Returning empty keymap?  No, default keymap.
void parse_keymap_cfg(YAML::Node node, program& prog) {
  if (!node) return;
  if (!node.IsMap())
    throw("keymap node is not a map");

  for (auto it : node) {
    string keystr = it.first.as<string>();
    keycodes key = string_to_key(keystr);
    if (it.second.IsMap()) {
      // handle macro assigned to key
      cout << "XXX unhandled macro\n";
    }
    else {
      string emitstr = it.second.as<string>();
      keycodes emit = string_to_key(emitstr);
      prog.kmap.assign(key, emit);
    }
  }
}


// Parse config such as
//
// macros:
// - id: 1
//   mode: once   # hold, toggle
//   steps: [down:A, ms:5, up:A, ms:5]
//
// A list of macros, each with id, mode, and set of steps.  Each step is an
// event, one of down key, up key, or delay.  Value is the key or delay in
// milliseconds.
vector<cus_macro> parse_macros_cfg(YAML::Node node) {
  vector<cus_macro> macros;
  for (auto macronode : node) {
    cus_macro macro;
    macro.id = macros.size();
    // cout << "XXX dumping macro mode:\n";
    // dump(macronode["mode"]);

    string mode = macronode["mode"].as<string>("once");
    if (mode == "once") macro.play_mode = 0;
    if (mode == "hold") macro.play_mode = 1;
    if (mode == "toggle") macro.play_mode = 2;
    
    vector<string> bits;
    for (auto it : macronode["steps"]) {
      // cout << "XXX dumping macro steps:\n";
      // dump(it);
      string val = it.as<string>();
      split(bits, val, is_space(), token_compress_on);
      if (bits.size() != 2)
	throw runtime_error("Bad macro step format: should be keyname/int down/up/ms");
      
      cus_macro_step step;
      string event = bits[1];
      if (event == "ms")
	step.duration = stoi(bits[0]);
      else {
	step.key = string_to_key(bits[0]);
	step.down = (event == "down");
	step.duration = 0;
      }
      macro.steps.push_back(step);
    }
    macros.push_back(macro);
  }
  return macros;
}

void parse_flashlight_cfg(YAML::Node node) {

}

// Parse the lights stanza.  Driver layer uses a sequence of color frames,
// under colormap.  Custom layers use animation and pattern frames.
void parse_light_cfg(YAML::Node cfg, program& prog) {
  auto cm = cfg["colormap"];
  auto anim = cfg["animation"];
  auto lite = cfg["pattern"];

  if (cm) {
    // read sequence of key color frames
    cout << "light colormap has " << cm.size() << " frames\n";
    // Parse a set of colorframes
    int delay = 0;
    for (size_t i=0; i < cm.size(); i++) {
      if (cm[i].IsScalar()) {
	delay = cm[i].as<int>();
	continue;
      }
      
      rgb_frame frame;
      frame.delay = delay;
      delay = 0;
      auto cmframe = cm[i];
      for (auto it: cmframe) {
	string key = it.first.as<string>();
	string color = it.second.as<string>();
	keycodes keyc = string_to_key(key);

	auto rgbiter = colornames.find(lower(color));
	if (rgbiter == colornames.end()) {
	  string msg = "Illegal color name specified: " + color;
	  throw runtime_error(msg);
	}
	rgb RGB = rgbiter->second;
	frame.setkey(keyc, RGB);
	// cout << "Frame " << i << " " << key << " -> " << color;
	// cout << "  codes k: " << keyc << " c: " << RGB.R << " " << RGB.G << " " << RGB.B << endl;
      }
      prog.lights.push_back(frame);
    }

    // return lights
    
  }
  if (anim) {
    cout << "lighting has " << anim.size() << " animation frames\n";
    // Parse a set of animation frames
    for (size_t i=0; i < anim.size(); i++) {
      animation_frame frame;
      if (!anim[i].IsSequence())
	throw runtime_error("Bad format for animation frame");
      // build a key bitmap
      for (auto keynode : anim[i]) {
	keycodes key = string_to_key(keynode.as<string>());
	frame.enable(key);
      }
      prog.custom_lights.aframes.push_back(frame);
    }
    // return lights
  }
  if (lite) {
    cout << "lights has " << lite.size() << " lighting frames\n";
    // Parse a set of lighting frames
    for (size_t i=0; i < lite.size(); i++) {
      pattern_frame frame;
      
      auto knode = lite[i]["keys"];

      // Parse the color
      auto cnode = lite[i]["color"];
      rgb RGB = node_to_color(cnode);

      // Parse the lighting pattern and duration
      auto pattern = lite[i]["type"];
      auto duration = lite[i]["duration"];
      auto gapnode = lite[i]["gap"];
      string type = pattern.as<string>();
      if (type == "monochrome") {
	frame.monochrome(RGB.R, RGB.G, RGB.B);
      }
      else if (type == "cycle") {
	if (!duration)
	  throw runtime_error("missing duration field");
	int dur = duration.as<int>();
	frame.rgb_cycle(RGB.R, RGB.G, RGB.B, dur);
      }
      else if (type == "breathe") {
	if (!duration)
	  throw runtime_error("missing duration field");
	if (!gapnode)
	  throw runtime_error("missing gap field");
	int dur = duration.as<int>();
	int gap = gapnode.as<int>();
	frame.breathe(RGB.R, RGB.G, RGB.B, dur, gap);
      }
      else
	throw runtime_error(string("Unknown light frame pattern " + type));
      
      prog.custom_lights.lframes.push_back(frame);
    }
  }
}


// Treat driver mode and custom mode configs as separate streams
program read_config(istream& is) {
  YAML::Node cfg = YAML::Load(is);

  if (!cfg["layer"])
    throw runtime_error("No layer defined");

  program prog;
  parse_keymap_cfg(cfg["keymap"], prog);
  parse_light_cfg(cfg["lights"], prog);
  parse_macros_cfg(cfg["macros"]);
  parse_flashlight_cfg(cfg["flashlight"]);
  
  // validate();

  // Doesn't seem to be able to read multiple docs correctly from the same yaml file
  return prog;
}


void init_parse() {
  init_namemap();
  load_colornames();
}
