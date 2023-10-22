#ifndef KEYMAP_HH
#define KEYMAP_HH

#include <cstdint>

// Basic keymap data structures

struct key {
  int code;
  int rgb;
};

// Semi-arbitrary codes for each key.  These get mapped multiple ways
// depending on light program vs keymapping, driver vs custom, etc.
enum keycodes {
  // 0 is reserved
  K_NONE,
  // Letters
  K_A=1, K_B, K_C, K_D, K_E, K_F, K_G, K_H,
  K_I=9, K_J, K_K, K_L, K_M, K_N, K_O, K_P,
  K_Q=17, K_R, K_S, K_T, K_U, K_V, K_W, K_X,
  K_Y=25, K_Z,
  // Numbers
  K_1=27, K_2, K_3, K_4, K_5, K_6, K_7, K_8,
  K_9, K_0,
  // Other printing chars
  K_Dash=37, K_Equal, K_LBracket, K_RBracket,
  K_BkSlash=41, K_Semi, K_Quote, K_Comma, K_Period, K_Slash,
  K_Backquote=47,
  // Nonprinting keys
  K_Esc=48, K_Tab, K_Capslock,
  K_F1=51, K_F2, K_F3, K_F4, K_F5, K_F6,
  K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
  K_REnter=63, K_MEnter,
  K_LShft, K_MShft, K_RShft,
  K_LCtrl, K_MCtrl, K_RCtrl,
  K_LAlt=71, K_RAlt=72,
  K_MBksp=73, K_RBksp=74,
  K_LSpc, K_RSpc,
  K_Win,
  K_PgUp=78, K_PgDn,
  K_Left, K_Right, K_Up, K_Down,
  K_PrtScrn=84, K_Del,
  K_Xbows=86,			// Light only
  // Numpad
  K_Numlock=87, K_NSlash, K_NStar, K_NEnter,
  K_N1=91, K_N2, K_N3, K_N4, K_N5, K_N6, K_N7, K_N8, K_N9, K_N0,
  K_NPeriod=101, K_NMinus, K_NPlus, 
  // Media
  K_MediaPlay=104, K_MediaPause, K_MediaStop, K_MediaLast, K_MediaNext,
  K_MediaVolUp=109, K_MediaVolDown, K_MediaMute,
  // Mouse
  K_LClick=112, K_MClick, K_RClick, K_BackClick, K_FwdClick,
  // Sys/net
  K_NetBack=117, K_NetFwd, K_NetRefresh, K_NetCollection, K_NetHome,
  K_NetEmail=122, K_NetComp, K_NetCalc, K_NetCopy, K_NetPaste, K_NetPrtScrn,
  K_Home=128, K_End, K_Ins,		// Not on xbows kbd
  K_102ND=131,
  K_Fn,
  MAX_KEYCODE
};

// Map from keycode to key id.  Used to build driver keymap and custom keymap.
extern uint32_t keyid[MAX_KEYCODE];


#endif
