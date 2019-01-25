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
  K_Fn,
  MAX_KEYCODE
};

// Map from keycode to key id.  Used to build driver keymap and custom keymap.
extern uint16_t keyid[MAX_KEYCODE];

// // Order of keys in driver mode light program
// int drv_light_pos[] =
//   {
//     // Group 1
//     K_Esc, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6,
//     K_NONE, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
//     // Group 2
//     K_Del, K_NONE, K_PrtScrn, K_NONE, K_NONE, K_NONE,
//     K_NONE, K_NONE, K_Backquote, K_1, K_2, K_NONE, K_3, K_4,
//     // Group 3
//     K_5, K_NONE, K_6, K_7, K_8, K_NONE, K_9, K_0, K_Dash, K_Equal,
//     K_Bksp, K_NONE, K_NONE, K_NONE,
//     // Group 4
//     K_NONE, K_NONE,
//     K_Tab, K_Q, K_W, K_NONE, K_E, K_R, K_T, K_Xbows, K_Y, K_U, K_I, K_NONE,
//     // Group 5
//     K_O, K_P, K_LBracket, K_RBracket, K_BkSlash, K_PgUp,
//     K_NONE, K_NONE, K_NONE, K_NONE, K_Capslock, K_A, K_S, K_NONE,
//     // Group 6
//     K_D, K_F, K_G, K_MBksp, K_H, K_J, K_K, K_NONE, K_L, K_Semi,
//     K_Quote, K_REnter, K_NONE, K_PgDn,
//     // Group 7
//     K_NONE, K_NONE, K_NONE, K_NONE, K_LShft, K_Z, K_X, K_NONE,
//     K_C, K_V, K_B, K_MEnter, K_N, K_M,
//     // Group 8
//     K_Comma, K_NONE, K_Period, K_Slash, K_RShft, K_NONE, K_Up,
//     K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_LCtrl, K_Win,
//     // Group 9
//     K_LAlt, K_NONE, K_NONE, K_LSpc, K_MCtrl, K_NONE, K_MShft, K_RSpc, K_RAlt,
//     K_NONE, K_NONE, K_Fn, K_RCtrl, K_Left,
//     // Group 10 - only 24 valid bytes of 56
//     K_Down, K_Right, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE
//   };

// // Key that is present at each position, indexed by position
// int drv_keymap_pos[] =
//   {
//     // Group 1
//     K_NONE, K_NONE,
//     K_LShft, K_LCtrl, K_LAlt, K_Win,
//     K_RCtrl, K_RShft, K_RAlt, K_NONE,
//     K_A, K_B, K_C, K_D,
//     // Group 2
//     K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N, K_O, K_P, K_Q, K_R,
//     // Group 3
//     K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z, K_1, K_2, K_3, K_4, K_5, K_6,
//     // Group 4
//     K_7, K_8, K_9, K_0, K_REnter, K_Esc, K_RBksp, K_Tab, K_LSpc,
//     K_Dash, K_Equal, K_LBracket, K_RBracket, K_BkSlash,
//     // Group 5,
//     K_Semi, K_Quote, K_Backquote, K_Comma, K_Period, K_Slash, K_Capslock,
//     K_F1, K_F2, K_F3, K_F4, K_F6, K_F7,
//     // Group 6,
//     K_F8, K_F9, K_F10, K_F11, K_F12, K_PrtScrn, K_NONE, K_NONE,
//     K_NONE, K_NONE, K_PgUp, K_Del, K_NONE, K_PgDn,
//     // Group 7
//     K_Right, K_Left, K_Down, K_Up, K_NONE, K_NONE, K_NONE, K_NONE,
//     K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
//     // Group 8,
//     K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
//     K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
//     // Group 9,
//     K_MBksp, K_MEnter, K_MCtrl, K_MShft, K_RSpc,
//     K_NONE, K_NONE,
//     K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
//   };

// // Code used to represent key at each position, indexed by keycode.
// // All lead with 0x02.
// int drv_keymap_code[] =
//   {
//     0,
//     0x00000400, 0x00000500, 0x00000600, 0x00000700, // A B C D
//     0x00000800, 0x00000900, 0x00000a00, 0x00000b00, // E F G H
//     0x00000c00, 0x00000d00, 0x00000e00, 0x00000f00, // I J K L
//     0x00001000, 0x00001100, 0x00001200, 0x00001300, // M N O P
//     0x00001400, 0x00001500, 0x00001600, 0x00001700, // Q R S T
//     0x00001800, 0x00001900, 0x00001a00, 0x00001b00, // U V W X
//     0x00001c00, 0x00001d00, 0x00001e00, 0x00001f00, // Y Z 1 2
//     0x00002000, 0x00002100, 0x00002200, 0x00002300, // 3 4 5 6
//     0x00002400, 0x00002500, 0x00002600, 0x00002700, // 7 8 9 0
//     0x00002d00,	0x00002e00, 0x00002f00, 0x00003000, // -_ =+ [{ ]}
//     0x00003100, 0x00003300, 0x00003400, 0x00003600, // \| ;: '" ,<
//     0x00003700,	0x00003800, 0x00003500, 0x00002900, // .> /? `~ Esc
//     0x00002b00, 0x00003900, 0x00003a00, 0x00003b00, // Tab Caps F1 F2
//     0x00003c00, 0x00003d00, 0x00003e00, 0x00003f00, // F3 F4 F5 F6
//     0x00004000, 0x00004100, 0x00004200, 0x00004300, // F7 F8 F9 F10
//     0x00004400, 0x00004500,			    // F11 F12
//     0x00002800,	0x00002800,			    // REnt MEnt
//     0x00000001, 0x00000020, 0x00000020,		    // LShf MShf RShf
//     0x00000002, 0x00000010, 0x00000010,		    // LCtl MCtl RCtl
//     0x00000004, 0x00000040,			    // LAlt RAlt
//     0x00002a00, 0x00002a00,			    // MBS RBS
//     0x00000001, 0x00000020,			    // LSpc RSpc
//     0x00000008,	0x00004b00, 0x00004e00,		    // Win PgUp PgDn
//     0x00005000, 0x00004f00, 0x00005200, 0x00005100, // Left Rt Up Dn
//     0x00004600,	0x00004c00, 0, 0		    // PrtS Del xbow numlk
//     0, 0, 0,					    // np / * enter
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		    // np 1-9,0
//     0, 0, 0,					    // np .-+
//     0, 0, 0, 0, 0, 0, 0, 0,			    // media
//     0, 0, 0, 0, 0,				    // mouse
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		    // net
//     0, 0, 0					    // home end ins
//   };

#endif
