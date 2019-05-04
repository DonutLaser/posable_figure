#if !defined (WIN32_WINDOW_H)
#define WIN32_WINDOW_H

#include <windows.h>

#define KEY_LMB 0x01
#define KEY_RMB 0x02
#define KEY_CANCEL 0x03
#define KEY_MMB 0x04
#define KEY_XMB1 0x05
#define KEY_XMB2 0x06
#define KEY_BACKSPACE 0x08
#define KEY_TAB 0x09
#define KEY_CLEAR 0x0C
#define KEY_ENTER 0x0D
#define KEY_SHIFT 0x10
#define KEY_CTRL 0x11
#define KEY_ALT 0x12
#define KEY_PAUSE 0x13
#define KEY_CAPSLOCK 0x14
#define KEY_ESCAPE 0x1B
#define KEY_SPACE 0x20
#define KEY_PGUP 0x21
#define KEY_PGDOWN 0x22
#define KEY_END 0x23
#define KEY_HOME 0x24
#define KEY_LEFT 0x25
#define KEY_UP 0x26
#define KEY_RIGHT 0x27
#define KEY_DOWN 0x28
#define KEY_SELECT 0x29
#define KEY_PRINT 0x2A
#define KEY_PRTNSCR 0x2C
#define KEY_INSERT 0x2D
#define KEY_DELETE 0x2E
#define KEY_HELP 0x2F
#define KEY_ALPHA0 0x30
#define KEY_ALPHA1 0x31
#define KEY_ALPHA2 0x32
#define KEY_ALPHA3 0x33
#define KEY_ALPHA4 0x34
#define KEY_ALPHA5 0x35
#define KEY_ALPHA6 0x36
#define KEY_ALPHA7 0x37
#define KEY_ALPHA8 0x38
#define KEY_ALPHA9 0x39
#define KEY_A 0x41
#define KEY_B 0x42
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_F 0x46
#define KEY_G 0x47
#define KEY_H 0x48
#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_M 0x4D
#define KEY_N 0x4E
#define KEY_O 0x4F
#define KEY_P 0x50
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_T 0x54
#define KEY_U 0x55
#define KEY_V 0x56
#define KEY_W 0x57
#define KEY_X 0x58
#define KEY_Y 0x59
#define KEY_Z 0x5A
#define KEY_LWIN 0x5B
#define KEY_RWIN 0x5C
#define KEY_APPS 0x5D
#define KEY_SLEEP 0x5F
#define KEY_NUM0 0x60
#define KEY_NUM1 0x61
#define KEY_NUM2 0x62
#define KEY_NUM3 0x63
#define KEY_NUM4 0x64
#define KEY_NUM5 0x65
#define KEY_NUM6 0x66
#define KEY_NUM7 0x67
#define KEY_NUM8 0x68
#define KEY_NUM9 0x69
#define KEY_ASTERISK 0x6A
#define KEY_PLUS 0x6B
#define KEY_NUMENTER 0x6C
#define KEY_MINUS 0x6D
#define KEY_DECIMAL 0x6E
#define KEY_DIVIDE 0x6F
#define KEY_F1 0x70
#define KEY_F2 0x71
#define KEY_F3 0x72
#define KEY_F4 0x73
#define KEY_F5 0x74
#define KEY_F6 0x75
#define KEY_F7 0x76
#define KEY_F8 0x77
#define KEY_F9 0x78
#define KEY_F10 0x79
#define KEY_F11 0x7A
#define KEY_F12 0x7B
#define KEY_NUMLOCK 0x90
#define KEY_SCROLLLOCK 0x91
#define KEY_LSHIFT 0xA0
#define KEY_RSHIFT 0xA1
#define KEY_LCTRL 0xA2
#define KEY_RCTRL 0xA3
#define KEY_LALT 0xA4
#define KEY_RALT 0xA5
#define KEY_COLON 0xBA // OEM 1
#define KEY_EQUAL 0xBB // OEM PLUS
#define KEY_COMMA 0xBC // OEM COMMA
#define KEY_DASH 0xBD // OEM MINUS
#define KEY_DOT 0xBE // OEM PERIOD
#define KEY_QUESTION 0xBF // OEM 2
#define KEY_TILDE 0xC0 // OEM 3
#define KEY_LBRACE 0xDB // OEM 4
#define KEY_PIPE 0xDC // OEM 5
#define KEY_RBRACE 0xDD // OEM 6
#define KEY_QUOTE 0xDE // OEM 7

#define GUI_MAIN int CALLBACK WinMain (HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_arguments, int cmd_show)

struct win32_input {
	unsigned char current_keys_state[256]; 
	unsigned char last_keys_state[256];

	int mouse_x;
	int mouse_y;
	int last_mouse_x;
	int last_mouse_y;

	int mouse_scroll_direction;
};

struct win32_window {
	win32_input* input;

	HINSTANCE instance;
	HWND handle;

	bool quit;
	bool hide_and_lock_cursor;
	bool resized;
};

win32_window* window_new (const char* title, unsigned width, unsigned height);
bool window_update (win32_window* window);
void window_render (win32_window* window);
void window_close (win32_window* window);
void window_free (win32_window* window);
void window_set_icon (win32_window* window, const char* icon_path);

POINT get_window_size (win32_window* window);

double get_time ();

bool is_key_down (win32_window* window, unsigned key);
bool is_key_pressed (win32_window* window, unsigned key);
bool is_key_up (win32_window* window, unsigned key);

#endif