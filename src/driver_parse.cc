// This code is to parse a configuration for driver mode.  It is different
// from custom mode unfortunately.

#include <istream>
#include <string>
#include <unordered_map>

#include "keymap.hh"

using namespace std;

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


// Define a format
// Light frame is [key rgb ... key rgb]
// light program is sequence of frames, and gap between them
//
// keymap is [key assignment ... key assignment]
//
// macro is more complex

// allow color names?  If so, where to get them from?  Can I leverage X color
// tables?

void read_light_program(istream& os) {
  
}
