#pragma once

#define KEY_UNKNOWN                0x00    // 0

#define KEY_A                    0x04    // 4
#define KEY_B                    0x05    // 5
#define KEY_C                    0x06    // 6
#define KEY_D                    0x07    // 7
#define KEY_E                    0x08    // 8
#define KEY_F                    0x09    // 9
#define KEY_G                    0x0a    // 10
#define KEY_H                    0x0b    // 11
#define KEY_I                    0x0c    // 12
#define KEY_J                    0x0d    // 13
#define KEY_K                    0x0e    // 14
#define KEY_L                    0x0f    // 15
#define KEY_M                    0x10    // 16
#define KEY_N                    0x11    // 17
#define KEY_O                    0x12    // 18
#define KEY_P                    0x13    // 19
#define KEY_Q                    0x14    // 20
#define KEY_R                    0x15    // 21
#define KEY_S                    0x16    // 22
#define KEY_T                    0x17    // 23
#define KEY_U                    0x18    // 24
#define KEY_V                    0x19    // 25
#define KEY_W                    0x1a    // 26
#define KEY_X                    0x1b    // 27
#define KEY_Y                    0x1c    // 28
#define KEY_Z                    0x1d    // 29

#define KEY_1                    0x1e    // 30
#define KEY_2                    0x1f    // 31
#define KEY_3                    0x20    // 32
#define KEY_4                    0x21    // 33
#define KEY_5                    0x22    // 34
#define KEY_6                    0x23    // 35
#define KEY_7                    0x24    // 36
#define KEY_8                    0x25    // 37
#define KEY_9                    0x26    // 38
#define KEY_0                    0x27    // 39

/* Function keys */
#define KEY_ENTER                0x28    // 40
#define KEY_ESCAPE                0x29    // 41
#define KEY_BACKSPACE            0x2a    // 42
#define KEY_TAB                    0x2b    // 43
#define KEY_SPACE                0x2c    // 44
#define KEY_MINUS                0x2d    // 45	/* - */
#define KEY_EQUAL                0x2e    // 46	/* = */
#define KEY_LEFT_BRACKET        0x2f    // 47	/* [ */
#define KEY_RIGHT_BRACKET        0x30    // 48	/* ] */
#define KEY_BACKSLASH            0x31    // 49	/* \ */
#define KEY_HASHTILDE            0x32    // 50	//ISO USB keyboards
#define KEY_SEMICOLON            0x33    // 51	/* ; */
#define KEY_APOSTROPHE            0x34    // 52	/* ' */
#define KEY_GRAVE_ACCENT        0x35    // 53	/* ` */
#define KEY_COMMA                0x36    // 54	/* , */
#define KEY_PERIOD                0x37    // 55	/* . */
#define KEY_SLASH                0x38    // 56	/* / */

#define KEY_CAPS_LOCK            0x39    // 57
#define KEY_F1                    0x3a    // 58
#define KEY_F2                    0x3b    // 59
#define KEY_F3                    0x3c    // 60
#define KEY_F4                    0x3d    // 61
#define KEY_F5                    0x3e    // 62
#define KEY_F6                    0x3f    // 63
#define KEY_F7                    0x40    // 64
#define KEY_F8                    0x41    // 65
#define KEY_F9                    0x42    // 66
#define KEY_F10                    0x43    // 67
#define KEY_F11                    0x44    // 68
#define KEY_F12                    0x45    // 69

#define KEY_PRINT_SCREEN        0x46    // 70
#define KEY_SCROLL_LOCK            0x47    // 71
#define KEY_PAUSE                0x48    // 72
#define KEY_INSERT                0x49    // 73
#define KEY_HOME                0x4a    // 74
#define KEY_PAGE_UP                0x4b    // 75
#define KEY_DELETE                0x4c    // 76
#define KEY_END                    0x4d    // 77
#define KEY_PAGE_DOWN            0x4e    // 78
#define KEY_RIGHT                0x4f    // 79
#define KEY_LEFT                0x50    // 80
#define KEY_DOWN                0x51    // 81
#define KEY_UP                    0x52    // 82
#define KEY_NUM_LOCK            0x53    // 83
#define KEY_KP_DIVIDE            0x54    // 84
#define KEY_KP_MULTIPLY            0x55    // 85
#define KEY_KP_SUBTRACT            0x56    // 86
#define KEY_KP_ADD                0x57    // 87
#define KEY_KP_ENTER            0x58    // 88
#define KEY_KP_1                0x59    // 89
#define KEY_KP_2                0x5a    // 90
#define KEY_KP_3                0x5b    // 91
#define KEY_KP_4                0x5c    // 92
#define KEY_KP_5                0x5d    // 93
#define KEY_KP_6                0x5e    // 94
#define KEY_KP_7                0x5f    // 95
#define KEY_KP_8                0x60    // 96
#define KEY_KP_9                0x61    // 97
#define KEY_KP_0                0x62    // 98
#define KEY_KP_DECIMAL            0x63    // 99	/* . */


#define KEY_F13                    0x68    // 104
#define KEY_F14                    0x69    // 105
#define KEY_F15                    0x6a    // 106
#define KEY_F16                    0x6b    // 107
#define KEY_F17                    0x6c    // 108
#define KEY_F18                    0x6d    // 109
#define KEY_F19                    0x6e    // 110
#define KEY_F20                    0x6f    // 111
#define KEY_F21                    0x70    // 112
#define KEY_F22                    0x71    // 113
#define KEY_F23                    0x72    // 114
#define KEY_F24                    0x73    // 115

#define KEY_OPEN                0x74    // 116	Keyboard Execute
#define KEY_HELP                0x75    // 117	Keyboard Help
#define KEY_PROPS                0x76    // 118	Keyboard Menu
#define KEY_FRONT                0x77    // 119	Keyboard Select
#define KEY_STOP                0x78    // 120	Keyboard Stop
#define KEY_AGAIN                0x79    // 121	Keyboard Again
#define KEY_UNDO                0x7a    // 122	Keyboard Undo
#define KEY_CUT                    0x7b    // 123	Keyboard Cut
#define KEY_COPY                0x7c    // 124	Keyboard Copy
#define KEY_PASTE                0x7d    // 125	Keyboard Paste
#define KEY_FIND                0x7e    // 126	Keyboard Find
#define KEY_MUTE                0x7f    // 127	Keyboard Mute
#define KEY_VOLUMEUP            0x80    // 128	Keyboard Volume Up
#define KEY_VOLUMEDOWN            0x81    // 129	Keyboard Volume Down


#define KEY_LEFT_CTRL            0xe0    // 224	Keyboard Left Control
#define KEY_LEFT_SHIFT            0xe1    // 225	Keyboard Left Shift
#define KEY_LEFT_ALT            0xe2    // 226	Keyboard Left Alt
#define KEY_LEFT_META            0xe3    // 227	Keyboard Left GUI
#define KEY_RIGHT_CTRL            0xe4    // 228	Keyboard Right Control
#define KEY_RIGHT_SHIFT            0xe5    // 229	Keyboard Right Shift
#define KEY_RIGHT_ALT            0xe6    // 230	Keyboard Right Alt
#define KEY_RIGHT_META            0xe7    // 231	Keyboard Right GUI

#define KEY_MEDIA_PLAYPAUSE        0xe8    // 232
#define KEY_MEDIA_STOPCD        0xe9    // 233
#define KEY_MEDIA_PREVIOUSSONG    0xea    // 234
#define KEY_MEDIA_NEXTSONG        0xeb    // 235
#define KEY_MEDIA_EJECTCD        0xec    // 236
#define KEY_MEDIA_VOLUMEUP        0xed    // 237
#define KEY_MEDIA_VOLUMEDOWN    0xee    // 238
#define KEY_MEDIA_MUTE            0xef    // 239
#define KEY_MEDIA_WWW            0xf0    // 240
#define KEY_MEDIA_BACK            0xf1    // 241
#define KEY_MEDIA_FORWARD        0xf2    // 242
#define KEY_MEDIA_STOP            0xf3    // 243
#define KEY_MEDIA_FIND            0xf4    // 244
#define KEY_MEDIA_SCROLLUP        0xf5    // 245
#define KEY_MEDIA_SCROLLDOWN    0xf6    // 246
#define KEY_MEDIA_EDIT            0xf7    // 247
#define KEY_MEDIA_SLEEP            0xf8    // 248
#define KEY_MEDIA_COFFEE        0xf9    // 249
#define KEY_MEDIA_REFRESH        0xfa    // 250
#define KEY_MEDIA_CALC            0xfb    // 251

#define BTN_LEFT                0x01    // 1
#define BTN_MIDDLE              0x02    // 2
#define BTN_RIGHT               0x03    // 3

#define KEY_LAST                KEY_MEDIA_CALC
#define BTN_LAST                BTN_RIGHT