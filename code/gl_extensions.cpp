#include "gl_extensions.h"

void* load_method (const char* name) {
	void* p = (void*)wglGetProcAddress (name);

	if (!p) {
		HMODULE module = LoadLibraryA ("opengl32.dll");
		p = (void*)GetProcAddress (module, name);
	}

	return p;
}

void gl_extensions_init () {
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)load_method ("wglChoosePixelFormatARB");
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)load_method ("wglCreateContextAttribsARB");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load_method ("glGenVertexArrays");
	glGenBuffers = (PFNGLGENBUFFERSPROC)load_method ("glGenBuffers");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load_method ("glBindVertexArray");
	glBindBuffer = (PFNGLBINDBUFFERPROC)load_method ("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)load_method ("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)load_method ("glBufferSubData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load_method ("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load_method ("glEnableVertexAttribArray");
	glCompileShader = (PFNGLCOMPILESHADERPROC)load_method ("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)load_method ("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load_method ("glGetShaderInfoLog");
	glCreateShader = (PFNGLCREATESHADERPROC)load_method ("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)load_method ("glShaderSource");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)load_method ("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)load_method ("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)load_method ("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load_method ("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load_method ("glGetProgramInfoLog");
	glDeleteShader = (PFNGLDELETESHADERPROC)load_method ("glDeleteShader");
	glUseProgram = (PFNGLUSEPROGRAMPROC)load_method ("glUseProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load_method ("glGetUniformLocation");
	glUniform3f = (PFNGLUNIFORM3FPROC)load_method ("glUniform3f");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load_method ("glUniformMatrix4fv");
}

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;