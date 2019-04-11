#include "win32_window.h"

#include "gl_extensions.h"
#include <gl/gl.h>

#define STARTING_INPUT_BUFFER_SIZE	40

static POINT wnd_get_size (win32_window* window) {
	RECT rect;
	GetWindowRect (window -> handle, &rect);

	return { rect.right - rect.left, rect.bottom - rect.top };
}

static POINT wnd_get_client_size (win32_window* window) {
	RECT rect;
	GetClientRect (window -> handle, &rect);

	return { rect.right, rect.bottom };
}

static bool initialize_opengl (HWND window) {
	HDC dc = GetDC (window);
	PIXELFORMATDESCRIPTOR format = { };
	format.nSize = sizeof (format);
	format.nVersion = 1;
	format.iPixelType = PFD_TYPE_RGBA;
	format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	format.cColorBits = 32;
	format.cAlphaBits = 8;
	format.iLayerType = PFD_MAIN_PLANE;

	int format_id = ChoosePixelFormat (dc, &format);
	PIXELFORMATDESCRIPTOR actual_format;
	DescribePixelFormat (dc, format_id, sizeof (actual_format), &actual_format);
	SetPixelFormat (dc, format_id, &actual_format);

	HGLRC gl_context = wglCreateContext (dc);
	if (!wglMakeCurrent (dc, gl_context))
		return false;
	
	gl_extensions_init();

	const int attrib_list[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};

	unsigned format_count;
	wglChoosePixelFormatARB (dc, attrib_list, NULL, 1, &format_id, &format_count);
	DescribePixelFormat (dc, format_id, sizeof (actual_format), &actual_format);
	SetPixelFormat (dc, format_id, &actual_format);

	const int ctx_attrib_list[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,	
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	gl_context = wglCreateContextAttribsARB (dc, NULL, ctx_attrib_list);
	if (!wglMakeCurrent (dc, gl_context))
		return false;

	ReleaseDC (window, dc);


	return true;
}

static LRESULT CALLBACK window_proc (HWND window, UINT msg, WPARAM w_param, LPARAM l_param) {
	win32_window* wnd = (win32_window*)GetWindowLongPtr (window, GWLP_USERDATA);

	if (wnd != NULL) {
		switch (msg) {
			case WM_SIZE: {
				glViewport (0, 0, LOWORD (l_param), HIWORD (l_param));
				return 0;
			}
			case WM_SETCURSOR: {
				if (!wnd -> hide_and_lock_cursor)
					break;

				if (LOWORD (l_param) == HTCLIENT) {
					SetCursor (NULL);
					return 0;
				}

				break;
			}
			case WM_CLOSE: {
				wnd -> quit = true;
				return 0;
			}
			case WM_DESTROY: {
				PostQuitMessage (0);
				return 0;
			}
		}
	}
	
	return DefWindowProc (window, msg, w_param, l_param);
}	

win32_window* window_new (const char* title, unsigned width, unsigned height) {
	win32_window* result = (win32_window*)malloc (sizeof (win32_window));

	result -> instance = GetModuleHandle (0);

	WNDCLASS new_class = { };
	new_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	new_class.lpfnWndProc = window_proc;
	new_class.hInstance = result -> instance;
	new_class.lpszClassName = title;

	if (RegisterClass (&new_class)) {
		result -> handle = CreateWindow (title, title, 
										 WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
										 CW_USEDEFAULT, CW_USEDEFAULT,
										 width, height, 
										 0, 0, result -> instance, 0);

		result -> quit = false;
		result -> hide_and_lock_cursor = false;

		if (result -> handle) {
			POINT size = wnd_get_size (result);
			POINT client_size = wnd_get_size (result);
			POINT difference = { size.x - client_size.x, size.y - client_size.y };

			POINT actual_size = { (long)width + difference.x, (long)height + difference.y };
			SetWindowPos (result -> handle, 0, 0, 0, (unsigned)actual_size.x, (unsigned)actual_size.y, 0);

			result -> input = (win32_input*)malloc (sizeof (win32_input));
			if (!result -> input) {
				// Error
				free (result);
			}

			for (unsigned i = 0; i < 256; ++i) {
				result -> input -> current_keys_state[i] = 0;
				result -> input -> last_keys_state[i] = 0;
			}

			if (!initialize_opengl (result -> handle)) {
				// Error
				free (result);
			}
		}
		else {
			// Error
			free (result);
		}
	}
	else {
		// Error
		free (result);
	}

	return result;
}

bool window_update (win32_window* window) {
	if (!IsWindowVisible (window -> handle)) {
		ShowWindow (window -> handle, SW_SHOW);

		// Before showing the window, store the actual window struct for use in window proc
		SetWindowLongPtr (window -> handle, GWLP_USERDATA, (LONG_PTR)window);
	}

	if (window -> quit)
		return false;

	for (unsigned i = 0; i < 256; ++i) {
		window -> input -> last_keys_state[i] = window -> input -> current_keys_state[i];
		window -> input -> current_keys_state[i] = 0;
	}

	if (!GetKeyboardState ((PBYTE)window -> input -> current_keys_state))
		; // Error

	if (is_key_down (window, KEY_LMB) || is_key_down (window, KEY_RMB))
		SetCapture (window -> handle);
	else if (is_key_up (window, KEY_LMB) || is_key_up (window, KEY_RMB))
		ReleaseCapture ();

	POINT mouse_pos;
	GetCursorPos (&mouse_pos);
	ScreenToClient (window -> handle, &mouse_pos);

	if (window -> hide_and_lock_cursor) {
		POINT size = wnd_get_client_size (window);
		window -> input -> last_mouse_x = size.x / 2;
		window -> input -> last_mouse_y = size.y / 2;

		// Store mouse delta when cursor is locked
		window -> input -> mouse_x = mouse_pos.x - window -> input -> last_mouse_x;
		window -> input -> mouse_y = mouse_pos.y - window -> input -> last_mouse_y;

		POINT mouse_screen_pos = { window -> input -> last_mouse_x, window -> input -> last_mouse_y };
		ClientToScreen (window -> handle, &mouse_screen_pos);
		SetCursorPos (mouse_screen_pos.x, mouse_screen_pos.y);
	}
	else {
		window -> input -> last_mouse_x = window -> input -> mouse_x;
		window -> input -> last_mouse_y = window -> input -> mouse_y;
		window -> input -> mouse_x = mouse_pos.x;
		window -> input -> mouse_y = mouse_pos.y;
	}

	window -> input -> mouse_scroll_direction = 0;

	MSG msg;
	while (PeekMessage (&msg, window -> handle, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_MOUSEWHEEL) {
			int delta = GET_WHEEL_DELTA_WPARAM (msg.wParam);
			window -> input -> mouse_scroll_direction = delta < 0 ? -1: (delta > 0 ? 1 : 0);
		}
		else {
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}
	
	return true;
}

void window_render (win32_window* window) {
	HDC dc = GetDC (window -> handle);
	SwapBuffers (dc);
	ReleaseDC (window -> handle, dc);
}

void window_close (win32_window* window) {
	window -> quit = true;
}

void window_free (win32_window* window) {
	free (window -> input);
}

POINT get_window_size (win32_window* window) {
	return wnd_get_client_size (window);
}

double get_time () {
	return GetTickCount () / 1000.0;
}

bool is_key_down (win32_window* window, unsigned key) {
	return ((window -> input -> current_keys_state[key] >> 7) & 1) == 1 &&
		   ((window -> input -> last_keys_state[key] >> 7) & 1) == 0;
}

bool is_key_pressed (win32_window* window, unsigned key) {
	return ((window -> input -> current_keys_state[key] >> 7) & 1) == 1 &&
		   ((window -> input -> last_keys_state[key] >> 7) & 1) == 1;
}

bool is_key_up (win32_window* window, unsigned key) {
	return ((window -> input -> current_keys_state[key] >> 7) & 1) == 0 &&
		   ((window -> input -> last_keys_state[key] >> 7) & 1) == 1;
}
