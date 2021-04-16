#pragma once

#include <unordered_map>

#ifdef _USE_GLFW

#define _KEY_UNKNOWN			-1

/* Printable keys */
#define _KEY_SPACE              32
#define _KEY_APOSTROPHE         39  /* ' */
#define _KEY_COMMA              44  /* , */
#define _KEY_MINUS              45  /* - */
#define _KEY_PERIOD             46  /* . */
#define _KEY_SLASH              47  /* / */
#define _KEY_0                  48
#define _KEY_1                  49
#define _KEY_2                  50
#define _KEY_3                  51
#define _KEY_4                  52
#define _KEY_5                  53
#define _KEY_6                  54
#define _KEY_7                  55
#define _KEY_8                  56
#define _KEY_9                  57
#define _KEY_SEMICOLON          59  /* ; */
#define _KEY_EQUAL              61  /* = */
#define _KEY_A                  65
#define _KEY_B                  66
#define _KEY_C                  67
#define _KEY_D                  68
#define _KEY_E                  69
#define _KEY_F                  70
#define _KEY_G                  71
#define _KEY_H                  72
#define _KEY_I                  73
#define _KEY_J                  74
#define _KEY_K                  75
#define _KEY_L                  76
#define _KEY_M                  77
#define _KEY_N                  78
#define _KEY_O                  79
#define _KEY_P                  80
#define _KEY_Q                  81
#define _KEY_R                  82
#define _KEY_S                  83
#define _KEY_T                  84
#define _KEY_U                  85
#define _KEY_V                  86
#define _KEY_W                  87
#define _KEY_X                  88
#define _KEY_Y                  89
#define _KEY_Z                  90
#define _KEY_LEFT_BRACKET       91  /* [ */
#define _KEY_BACKSLASH          92  /* \ */
#define _KEY_RIGHT_BRACKET      93  /* ] */
#define _KEY_GRAVE_ACCENT       96  /* ` */
#define _KEY_WORLD_1            161 /* non-US #1 */
#define _KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define _KEY_ESCAPE             256
#define _KEY_ENTER              257
#define _KEY_TAB                258
#define _KEY_BACKSPACE          259
#define _KEY_INSERT             260
#define _KEY_DELETE             261
#define _KEY_RIGHT              262
#define _KEY_LEFT               263
#define _KEY_DOWN               264
#define _KEY_UP                 265
#define _KEY_PAGE_UP            266
#define _KEY_PAGE_DOWN          267
#define _KEY_HOME               268
#define _KEY_END                269
#define _KEY_CAPS_LOCK          280
#define _KEY_SCROLL_LOCK        281
#define _KEY_NUM_LOCK           282
#define _KEY_PRINT_SCREEN       283
#define _KEY_PAUSE              284
#define _KEY_F1                 290
#define _KEY_F2                 291
#define _KEY_F3                 292
#define _KEY_F4                 293
#define _KEY_F5                 294
#define _KEY_F6                 295
#define _KEY_F7                 296
#define _KEY_F8                 297
#define _KEY_F9                 298
#define _KEY_F10                299
#define _KEY_F11                300
#define _KEY_F12                301
#define _KEY_F13                302
#define _KEY_F14                303
#define _KEY_F15                304
#define _KEY_F16                305
#define _KEY_F17                306
#define _KEY_F18                307
#define _KEY_F19                308
#define _KEY_F20                309
#define _KEY_F21                310
#define _KEY_F22                311
#define _KEY_F23                312
#define _KEY_F24                313
#define _KEY_F25                314
#define _KEY_KP_0               320
#define _KEY_KP_1               321
#define _KEY_KP_2               322
#define _KEY_KP_3               323
#define _KEY_KP_4               324
#define _KEY_KP_5               325
#define _KEY_KP_6               326
#define _KEY_KP_7               327
#define _KEY_KP_8               328
#define _KEY_KP_9               329
#define _KEY_KP_DECIMAL         330
#define _KEY_KP_DIVIDE          331
#define _KEY_KP_MULTIPLY        332
#define _KEY_KP_SUBTRACT        333
#define _KEY_KP_ADD             334
#define _KEY_KP_ENTER           335
#define _KEY_KP_EQUAL           336
#define _KEY_LEFT_SHIFT         340
#define _KEY_LEFT_CONTROL       341
#define _KEY_LEFT_ALT           342
#define _KEY_LEFT_SUPER         343
#define _KEY_RIGHT_SHIFT        344
#define _KEY_RIGHT_CONTROL      345
#define _KEY_RIGHT_ALT          346
#define _KEY_RIGHT_SUPER        347
#define _KEY_MENU               348

#define _KEY_LAST				_KEY_MENU

#endif //_USE_GLFW

#ifdef _USE_SDL2

#define _KEY_UNKNOWN			0

/* Printable keys */
#define _KEY_SPACE              44
#define _KEY_APOSTROPHE         52  /* ' */
#define _KEY_COMMA              54  /* , */
#define _KEY_MINUS              45  /* - */
#define _KEY_PERIOD             55  /* . */
#define _KEY_SLASH              56  /* / */
#define _KEY_SEMICOLON          51  /* ; */
#define _KEY_EQUAL              46  /* = */
#define _KEY_LEFT_BRACKET       47  /* [ */
#define _KEY_BACKSLASH          49  /* \ */
//#define _KEY_BACKSLASH		50  //ISO USB keyboards
#define _KEY_RIGHT_BRACKET      48  /* ] */
#define _KEY_GRAVE_ACCENT       53  /* ` */
#define _KEY_WORLD_1            161 /* non-US #1 */ //??
#define _KEY_WORLD_2            162 /* non-US #2 */ //??
#define _KEY_1                  30
#define _KEY_2                  31
#define _KEY_3                  32
#define _KEY_4                  33
#define _KEY_5                  34
#define _KEY_6                  35
#define _KEY_7                  36
#define _KEY_8                  37
#define _KEY_9                  38
#define _KEY_0                  39

#define _KEY_A                  4
#define _KEY_B                  5
#define _KEY_C                  6
#define _KEY_D                  7
#define _KEY_E                  8
#define _KEY_F                  9
#define _KEY_G                  10
#define _KEY_H                  11
#define _KEY_I                  12
#define _KEY_J                  13
#define _KEY_K                  14
#define _KEY_L                  15
#define _KEY_M                  16
#define _KEY_N                  17
#define _KEY_O                  18
#define _KEY_P                  19
#define _KEY_Q                  20
#define _KEY_R                  21
#define _KEY_S                  22
#define _KEY_T                  23
#define _KEY_U                  24
#define _KEY_V                  25
#define _KEY_W                  26
#define _KEY_X                  27
#define _KEY_Y                  28
#define _KEY_Z                  29

/* Function keys */
#define _KEY_ESCAPE             41
#define _KEY_ENTER              40
#define _KEY_TAB                43
#define _KEY_BACKSPACE          42
#define _KEY_INSERT             73
#define _KEY_DELETE             76
#define _KEY_RIGHT              79
#define _KEY_LEFT               80
#define _KEY_DOWN               81
#define _KEY_UP                 82
#define _KEY_PAGE_UP            75
#define _KEY_PAGE_DOWN          78
#define _KEY_HOME               74
#define _KEY_END                77
#define _KEY_CAPS_LOCK          57
#define _KEY_SCROLL_LOCK        71
#define _KEY_NUM_LOCK           83
#define _KEY_PRINT_SCREEN       70
#define _KEY_PAUSE              72
#define _KEY_F1                 58
#define _KEY_F2                 59
#define _KEY_F3                 60
#define _KEY_F4                 61
#define _KEY_F5                 62
#define _KEY_F6                 63
#define _KEY_F7                 64
#define _KEY_F8                 65
#define _KEY_F9                 66
#define _KEY_F10                67
#define _KEY_F11                68
#define _KEY_F12                69
#define _KEY_F13                104
#define _KEY_F14                105
#define _KEY_F15                106
#define _KEY_F16                107
#define _KEY_F17                108
#define _KEY_F18                109
#define _KEY_F19                110
#define _KEY_F20                111
#define _KEY_F21                112
#define _KEY_F22                113
#define _KEY_F23                114
#define _KEY_F24                115
#define _KEY_F25                115
#define _KEY_KP_0               98
#define _KEY_KP_1               89
#define _KEY_KP_2               90
#define _KEY_KP_3               91
#define _KEY_KP_4               92
#define _KEY_KP_5               93
#define _KEY_KP_6               94
#define _KEY_KP_7               95
#define _KEY_KP_8               96
#define _KEY_KP_9               97
#define _KEY_KP_DECIMAL         99
#define _KEY_KP_DIVIDE          84
#define _KEY_KP_MULTIPLY        85
#define _KEY_KP_SUBTRACT        86
#define _KEY_KP_ADD             87
#define _KEY_KP_ENTER           88
#define _KEY_KP_EQUAL           336 //??
#define _KEY_LEFT_SHIFT         225
#define _KEY_LEFT_CONTROL       224
#define _KEY_LEFT_ALT           226
#define _KEY_LEFT_SUPER         343 //??
#define _KEY_RIGHT_SHIFT        229
#define _KEY_RIGHT_CONTROL      228
#define _KEY_RIGHT_ALT          230
#define _KEY_RIGHT_SUPER        347 //??
#define _KEY_MENU               118

#define _KEY_LAST				_KEY_RIGHT_ALT

#endif //_USE_SDL2

#ifndef _USE_GLFW
#ifndef _USE_SDL2
#error Must specify a input keycode mapping
#endif
#endif

#ifndef _USE_SDL2
#ifndef _USE_GLFW
#error Must specify a input keycode mapping
#endif
#endif

/* The unknown key */
#define KEY_UNKNOWN			_KEY_UNKNOWN

/* Printable keys */
#define KEY_SPACE			_KEY_SPACE
#define KEY_APOSTROPHE		_KEY_APOSTROPHE		/* ' */
#define KEY_COMMA			_KEY_COMMA			/* , */
#define KEY_MINUS			_KEY_MINUS			/* - */
#define KEY_PERIOD			_KEY_PERIOD			/* . */
#define KEY_SLASH			_KEY_SLASH			/* / */
#define KEY_SEMICOLON		_KEY_SEMICOLON		/* ; */
#define KEY_EQUAL			_KEY_EQUAL			/* = */
#define KEY_LEFT_BRACKET	_KEY_LEFT_BRACKET	/* [ */
#define KEY_BACKSLASH		_KEY_BACKSLASH		/* \ */
#define KEY_RIGHT_BRACKET	_KEY_RIGHT_BRACKET	/* ] */
#define KEY_GRAVE_ACCENT	_KEY_GRAVE_ACCENT	/* ` */
#define KEY_WORLD_1			_KEY_WORLD_1		/* non-US #1 */
#define KEY_WORLD_2			_KEY_WORLD_2		/* non-US #2 */

#define KEY_1				_KEY_1
#define KEY_2				_KEY_2
#define KEY_3				_KEY_3
#define KEY_4				_KEY_4
#define KEY_5				_KEY_5
#define KEY_6				_KEY_6
#define KEY_7				_KEY_7
#define KEY_8				_KEY_8
#define KEY_9				_KEY_9
#define KEY_0				_KEY_0

#define KEY_A				_KEY_A
#define KEY_B				_KEY_B
#define KEY_C				_KEY_C
#define KEY_D				_KEY_D
#define KEY_E				_KEY_E
#define KEY_F				_KEY_F
#define KEY_G				_KEY_G
#define KEY_H				_KEY_H
#define KEY_I				_KEY_I
#define KEY_J				_KEY_J
#define KEY_K				_KEY_K
#define KEY_L				_KEY_L
#define KEY_M				_KEY_M
#define KEY_N				_KEY_N
#define KEY_O				_KEY_O
#define KEY_P				_KEY_P
#define KEY_Q				_KEY_Q
#define KEY_R				_KEY_R
#define KEY_S				_KEY_S
#define KEY_T				_KEY_T
#define KEY_U				_KEY_U
#define KEY_V				_KEY_V
#define KEY_W				_KEY_W
#define KEY_X				_KEY_Y
#define KEY_Y				_KEY_X
#define KEY_Z				_KEY_Z

/* Function keys */
#define KEY_ESCAPE			_KEY_ESCAPE
#define KEY_ENTER			_KEY_ENTER
#define KEY_TAB				_KEY_TAB
#define KEY_BACKSPACE		_KEY_BACKSPACE
#define KEY_INSERT			_KEY_INSERT
#define KEY_DELETE			_KEY_DELETE
#define KEY_RIGHT			_KEY_RIGHT
#define KEY_LEFT			_KEY_LEFT
#define KEY_DOWN			_KEY_DOWN
#define KEY_UP				_KEY_UP
#define KEY_PAGE_UP			_KEY_PAGE_UP
#define KEY_PAGE_DOWN		_KEY_PAGE_DOWN
#define KEY_HOME			_KEY_HOME
#define KEY_END				_KEY_END
#define KEY_CAPS_LOCK		_KEY_CAPS_LOCK
#define KEY_SCROLL_LOCK		_KEY_SCROLL_LOCK
#define KEY_NUM_LOCK		_KEY_NUM_LOCK
#define KEY_PRINT_SCREEN	_KEY_PRINT_SCREEN
#define KEY_PAUSE			_KEY_PAUSE
#define KEY_F1				_KEY_F1
#define KEY_F2				_KEY_F2
#define KEY_F3				_KEY_F3
#define KEY_F4				_KEY_F4
#define KEY_F5				_KEY_F5
#define KEY_F6				_KEY_F6
#define KEY_F7				_KEY_F7
#define KEY_F8				_KEY_F8
#define KEY_F9				_KEY_F9
#define KEY_F10				_KEY_F10
#define KEY_F11				_KEY_F11
#define KEY_F12				_KEY_F12
#define KEY_F13				_KEY_F13
#define KEY_F14				_KEY_F14
#define KEY_F15				_KEY_F15
#define KEY_F16				_KEY_F16
#define KEY_F17				_KEY_F17
#define KEY_F18				_KEY_F18
#define KEY_F19				_KEY_F19
#define KEY_F20				_KEY_F20
#define KEY_F21				_KEY_F21
#define KEY_F22				_KEY_F22
#define KEY_F23				_KEY_F23
#define KEY_F24				_KEY_F24
#define KEY_F25				_KEY_F25
#define KEY_KP_0			_KEY_KP_0
#define KEY_KP_1			_KEY_KP_1
#define KEY_KP_2			_KEY_KP_2
#define KEY_KP_3			_KEY_KP_3
#define KEY_KP_4			_KEY_KP_4
#define KEY_KP_5			_KEY_KP_5
#define KEY_KP_6			_KEY_KP_6
#define KEY_KP_7			_KEY_KP_7
#define KEY_KP_8			_KEY_KP_8
#define KEY_KP_9			_KEY_KP_9
#define KEY_KP_DECIMAL		_KEY_KP_DECIMAL
#define KEY_KP_DIVIDE		_KEY_KP_DIVIDE
#define KEY_KP_MULTIPLY		_KEY_KP_MULTIPLY
#define KEY_KP_SUBTRACT		_KEY_KP_SUBTRACT
#define KEY_KP_ADD			_KEY_KP_ADD
#define KEY_KP_ENTER		_KEY_KP_ENTER
#define KEY_KP_EQUAL		_KEY_KP_EQUAL
#define KEY_LEFT_SHIFT		_KEY_LEFT_SHIFT
#define KEY_LEFT_CONTROL	_KEY_LEFT_CONTROL
#define KEY_LEFT_ALT		_KEY_LEFT_ALT
#define KEY_LEFT_SUPER		_KEY_LEFT_SUPER
#define KEY_RIGHT_SHIFT		_KEY_RIGHT_SHIFT
#define KEY_RIGHT_CONTROL	_KEY_RIGHT_CONTROL
#define KEY_RIGHT_ALT		_KEY_RIGHT_ALT
#define KEY_RIGHT_SUPER		_KEY_RIGHT_SUPER
#define KEY_MENU			_KEY_MENU

#define KEY_ESCAPE	_KEY_ESCAPE

#define KEY_LAST	_KEY_LAST

#define KEY_IS_DOWN	1

namespace PB
{
	void Init_CharMap();
	int8_t GetCharFromCode(uint8_t code);
}