#include "Arduino.h"
#include "BleComboKeyboard.h"

#define PLACEHOLDER	255

// const uint8_t KeyCode[] = {
// 	'1', '4', '7', '*', ' ', ' ', ' ',
// 	'2', '5', '8', '0', ' ', ' ', ' ',
// 	'3', '6', '9', '#', ' ', ' ', ' ',
// 	'A', 'B', 'C', 'D', ' ', ' ', ' '
// };


/*const*/ uint8_t KeyCode[] = {
// 0 ~ 4
	KEY_ESC, '`', KEY_TAB, KEY_CAPS_LOCK, KEY_LEFT_SHIFT,
// 5 ~ 9
	KEY_LEFT_CTRL, PLACEHOLDER, KEY_F1, '1', 'q',
// 10 ~ 14
	'a', 'z', PLACEHOLDER, PLACEHOLDER, KEY_F2,
// 15 ~ 19
	'2', 'w', 's', 'x', KEY_LEFT_ALT,
// 20 ~ 24
	PLACEHOLDER, KEY_F3, '3', 'e', 'd',
// 25 ~ 29
	'c', PLACEHOLDER, PLACEHOLDER, KEY_F4, '4',
// 30 ~ 34
	'r', 'f', 'v', ' ', PLACEHOLDER,
// 35 ~ 39
	KEY_F5, '5', 't', 'g', 'b',
// 40 ~ 44
// 	PLACEHOLDER, PLACEHOLDER, PLACEHOLDER, PLACEHOLDER, PLACEHOLDER,
// // 45 ~ 49
// 	PLACEHOLDER, PLACEHOLDER, PLACEHOLDER, PLACEHOLDER, PLACEHOLDER,
// // 50 ~ 54
};