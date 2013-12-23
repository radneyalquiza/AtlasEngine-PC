// Open GL Class implementation

#include "OpenGLClass.h"

// default constructor
OpenGLClass::OpenGLClass() {
	dcontext = 0;
	rcontext = 0;
}

// copy
OpenGLClass::OpenGLClass(const OpenGLClass& src) {

}

// destructor
OpenGLClass::~OpenGLClass() {

}


/* =========================================================================================== */
// Initialize Extensions
/* =========================================================================================== */
bool OpenGLClass::initializeExtensions(HWND hwndin) {

	HDC devcontext;
	PIXELFORMATDESCRIPTOR pformat;
	int error;
	HGLRC rendcontext;
	bool result;

	// get device context for this window
	devcontext = GetDC(hwndin);
	if(!devcontext)
		return false;

	// set a temporary pixel format
	error = SetPixelFormat(devcontext, 1, &pformat);	// returns an integer state
	if(error != 1)
		return false;

	// create the temporary rendering context
	rendcontext = wglCreateContext(devcontext);
	if(!rendcontext)
		return false;

	// set the current rendering context as the current context for the window
	error = wglMakeCurrent(devcontext, rendcontext);
	if(error != 1)
		return false;

	// initialize opengl extensions now
	result = LoadExtensionList();
	if(!result)
		return false;

	// release all temporary contexts
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rendcontext);
	rendcontext = NULL;

	ReleaseDC(hwndin, devcontext);
	devcontext = 0;

	return true;
}

/* =========================================================================================== */
// Initialize OpenGL
/* =========================================================================================== */
bool OpenGLClass::initializeOpenGL(HWND hwndin, int width, int height, float depth, float nearf, bool vsync) {

	int attributeListInt[19];
	int pformat[1];
	unsigned int formatCount;
	int result;
	PIXELFORMATDESCRIPTOR pdesc;
	int attributeList[5];
	float fov, aspect;
	char *vendorstring, *rendererstring;

	// get the device context
	dcontext = GetDC(hwndin);
	if(!dcontext)
		return false;

	// support for opengl rendering
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	// Support for rendering to a window.
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	// Support for hardware acceleration.
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	// Support for 24bit color.
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;

	// Support for 24 bit depth buffer.
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;

	// Support for double buffer.
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	// Support for swapping front and back buffer.
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	// Support for the RGBA pixel type.
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	// Support for a 8 bit stencil buffer.
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;

	// end the attribute list
	attributeListInt[18] = 0;

	// query for a pixel format that fits into the attributes we want.
	result = wglChoosePixelFormatARB(dcontext, attributeListInt, NULL, 1, pformat, &formatCount);
	if(!result)
		return false;

	// if video card can handler the desired pixel format, set it
	result = SetPixelFormat(dcontext, pformat[0], &pdesc);
	if(result != 1)
		return false;

	// set the 4.0 version of the opengl in the attribute list
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;

	// null terminate
	attributeList[4] = 0;

	// create rendering context
	rcontext = wglCreateContextAttribsARB(dcontext, 0, attributeList);
	if(rcontext == NULL)
		return false;

	// set the rendering context to active
	result = wglMakeCurrent(dcontext, rcontext);
	if(result != 1)
		return false;

	// set the depth buffer to all 1.0f
	glClearDepth(1.0f);

	// enable depth test
	glEnable(GL_DEPTH_TEST);

	// set the polygon winding to front facing (left handed)
	glFrontFace(GL_CW);

	// enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// initialize the world/model matrix to the identity
	BuildIdentityMatrix(worldMatrix);

	// set the fov and aspect ratio
	fov = 3.14159265358979323846f / 4.0f;	// high precision
	aspect = (float)width/(float)height;

	// build the projection matrix
	BuildPerspectiveFovLHMatrix(projMatrix, fov, aspect, nearf, depth);

	// get the video card name
	vendorstring = (char*)glGetString(GL_VENDOR);
	rendererstring = (char*)glGetString(GL_RENDERER);

	// store the video card name in the class member var
	strcpy_s(videoCardDesc, vendorstring);
	strcpy_s(videoCardDesc, "-");
	strcpy_s(videoCardDesc, rendererstring);

	// turn vertical sync on/off
	if(vsync)
		result = wglSwapIntervalEXT(1);
	else
		result = wglSwapIntervalEXT(0);

	// check the vsync set status
	if(!result)
		return false;
}

/* =========================================================================================== */
// Shutdown opengl
/* =========================================================================================== */
void OpenGLClass::shutdown(HWND hwnd) {
	
	// release the rendering context
	if(rcontext)
	{
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(rcontext);
		rcontext = 0;
	}

	// release device context
	if(dcontext)
	{
		ReleaseDC(hwnd, dcontext);
		dcontext = 0;
	}
}

/* =========================================================================================== */
// HELPER - Begin Scene
/* =========================================================================================== */
void OpenGLClass::beginScene(float red, float green, float blue, float alpha) {

	// set the color to clear the screen to.
	glClearColor(red, green, blue, alpha);

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* =========================================================================================== */
// HELPER - End Scene
/* =========================================================================================== */
void OpenGLClass::endScene() {

	// swap the back buffer to show to the monitor (since building the scene has completed)
	SwapBuffers(dcontext);
}

/* =========================================================================================== */
// Load All usable extensions for OpenGL (add more if you need more functionality)
/* =========================================================================================== */
bool OpenGLClass::LoadExtensionList() {

	// Load the OpenGL extensions that this application will be using.
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if(!wglChoosePixelFormatARB)
	{
		return false;
	}

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if(!wglCreateContextAttribsARB)
	{
		return false;
	}

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(!wglSwapIntervalEXT)
	{
		return false;
	}

	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if(!glAttachShader)
	{
		return false;
	}

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if(!glBindBuffer)
	{
		return false;
	}

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if(!glBindVertexArray)
	{
		return false;
	}

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if(!glBufferData)
	{
		return false;
	}

	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if(!glCompileShader)
	{
		return false;
	}

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if(!glCreateProgram)
	{
		return false;
	}

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if(!glCreateShader)
	{
		return false;
	}

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if(!glDeleteBuffers)
	{
		return false;
	}

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if(!glDeleteProgram)
	{
		return false;
	}

	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if(!glDeleteShader)
	{
		return false;
	}

	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if(!glDeleteVertexArrays)
	{
		return false;
	}

	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if(!glDetachShader)
	{
		return false;
	}

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if(!glEnableVertexAttribArray)
	{
		return false;
	}

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if(!glGenBuffers)
	{
		return false;
	}

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if(!glGenVertexArrays)
	{
		return false;
	}

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if(!glGetAttribLocation)
	{
		return false;
	}

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if(!glGetProgramInfoLog)
	{
		return false;
	}

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if(!glGetProgramiv)
	{
		return false;
	}

	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if(!glGetShaderInfoLog)
	{
		return false;
	}

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if(!glGetShaderiv)
	{
		return false;
	}

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if(!glLinkProgram)
	{
		return false;
	}

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if(!glShaderSource)
	{
		return false;
	}

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if(!glUseProgram)
	{
		return false;
	}

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if(!glVertexAttribPointer)
	{
		return false;
	}

	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	if(!glBindAttribLocation)
	{
		return false;
	}

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if(!glGetUniformLocation)
	{
		return false;
	}

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if(!glUniformMatrix4fv)
	{
		return false;
	}

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if(!glActiveTexture)
	{
		return false;
	}

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if(!glUniform1i)
	{
		return false;
	}

	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if(!glGenerateMipmap)
	{
		return false;
	}

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if(!glDisableVertexAttribArray)
	{
		return false;
	}

	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if(!glUniform3fv)
	{
		return false;
	}

	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if(!glUniform4fv)
	{
		return false;
	}
}

/* =========================================================================================== */
// Get World Matrix (attaches the values of the world matrix into a passed matrix)
/* =========================================================================================== */
void OpenGLClass::GetWorldMatrix(float* matrix) {

	// set every element in the worldmatrix array into the incoming matrix array
	matrix[0] = worldMatrix[0];
	matrix[1] = worldMatrix[1];
	matrix[2] = worldMatrix[2];
	matrix[3] = worldMatrix[3];
	matrix[4] = worldMatrix[4];
	matrix[5] = worldMatrix[5];
	matrix[6] = worldMatrix[6];
	matrix[7] = worldMatrix[7];
	matrix[8] = worldMatrix[8];
	matrix[9] = worldMatrix[9];
	matrix[10] = worldMatrix[10];
	matrix[11] = worldMatrix[11];
	matrix[12] = worldMatrix[12];
	matrix[13] = worldMatrix[13];
	matrix[14] = worldMatrix[14];
	matrix[15] = worldMatrix[15];
}

/* =========================================================================================== */
// Get Projection Matrix (attaches the values of the projection matrix into a passed matrix)
/* =========================================================================================== */
void OpenGLClass::GetProjectionMatrix(float* matrix) {

	matrix[0] = projMatrix[0];
	matrix[1] = projMatrix[1];
	matrix[2] = projMatrix[2];
	matrix[3] = projMatrix[3];
	matrix[4] = projMatrix[4];
	matrix[5] = projMatrix[5];
	matrix[6] = projMatrix[6];
	matrix[7] = projMatrix[7];
	matrix[8] = projMatrix[8];
	matrix[9] = projMatrix[9];
	matrix[10] = projMatrix[10];
	matrix[11] = projMatrix[11];
	matrix[12] = projMatrix[12];
	matrix[13] = projMatrix[13];
	matrix[14] = projMatrix[14];
	matrix[15] = projMatrix[15];
}

/* =========================================================================================== */
// Get Identity Matrix 
/* =========================================================================================== */
void OpenGLClass::BuildIdentityMatrix(float* matrix)
{
	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return;
}

/* =========================================================================================== */
// Build a perspective/projection matrix
/* =========================================================================================== */
void OpenGLClass::BuildPerspectiveFovLHMatrix(float* matrix, float fovin, float aspectin, float nearr, float depth) {

	matrix[0]  = 1.0f / (aspectin * tan(fovin * 0.5f));
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f / tan(fovin * 0.5f);
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = depth / (depth - nearr);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-nearr * depth) / (depth - nearr);
	matrix[15] = 0.0f;
}

/* =========================================================================================== */
// X Axis rotation matrix
/* =========================================================================================== */
void OpenGLClass::MatrixRotationX(float* matrix, float angle) {

}
/* =========================================================================================== */
// Y Axis rotation matrix
/* =========================================================================================== */
void OpenGLClass::MatrixRotationY(float* matrix, float angle) {

	matrix[0] = cosf(angle);
	matrix[1] = 0.0f;
	matrix[2] = -sinf(angle);
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = sinf(angle);
	matrix[9] = 0.0f;
	matrix[10] = cosf(angle);
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}
/* =========================================================================================== */
// Z Axis rotation matrix
/* =========================================================================================== */
void OpenGLClass::MatrixRotationZ(float* matrix, float angle) {

}

/* =========================================================================================== */
// Translation Matrix
/* =========================================================================================== */
void OpenGLClass::MatrixTranslation(float* matrix, float x, float y, float z) {

	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0f;
}

/* =========================================================================================== */
// Multiplying 2 Matrices
/* =========================================================================================== */
void OpenGLClass::MatrixMultiply(float* result, float* matrix1, float* matrix2) {

	result[0]  = (matrix1[0] * matrix2[0]) + (matrix1[1] * matrix2[4]) + (matrix1[2] * matrix2[8]) + (matrix1[3] * matrix2[12]);
	result[1]  = (matrix1[0] * matrix2[1]) + (matrix1[1] * matrix2[5]) + (matrix1[2] * matrix2[9]) + (matrix1[3] * matrix2[13]);
	result[2]  = (matrix1[0] * matrix2[2]) + (matrix1[1] * matrix2[6]) + (matrix1[2] * matrix2[10]) + (matrix1[3] * matrix2[14]);
	result[3]  = (matrix1[0] * matrix2[3]) + (matrix1[1] * matrix2[7]) + (matrix1[2] * matrix2[11]) + (matrix1[3] * matrix2[15]);

	result[4]  = (matrix1[4] * matrix2[0]) + (matrix1[5] * matrix2[4]) + (matrix1[6] * matrix2[8]) + (matrix1[7] * matrix2[12]);
	result[5]  = (matrix1[4] * matrix2[1]) + (matrix1[5] * matrix2[5]) + (matrix1[6] * matrix2[9]) + (matrix1[7] * matrix2[13]);
	result[6]  = (matrix1[4] * matrix2[2]) + (matrix1[5] * matrix2[6]) + (matrix1[6] * matrix2[10]) + (matrix1[7] * matrix2[14]);
	result[7]  = (matrix1[4] * matrix2[3]) + (matrix1[5] * matrix2[7]) + (matrix1[6] * matrix2[11]) + (matrix1[7] * matrix2[15]);

	result[8]  = (matrix1[8] * matrix2[0]) + (matrix1[9] * matrix2[4]) + (matrix1[10] * matrix2[8]) + (matrix1[11] * matrix2[12]);
	result[9]  = (matrix1[8] * matrix2[1]) + (matrix1[9] * matrix2[5]) + (matrix1[10] * matrix2[9]) + (matrix1[11] * matrix2[13]);
	result[10] = (matrix1[8] * matrix2[2]) + (matrix1[9] * matrix2[6]) + (matrix1[10] * matrix2[10]) + (matrix1[11] * matrix2[14]);
	result[11] = (matrix1[8] * matrix2[3]) + (matrix1[9] * matrix2[7]) + (matrix1[10] * matrix2[11]) + (matrix1[11] * matrix2[15]);

	result[12] = (matrix1[12] * matrix2[0]) + (matrix1[13] * matrix2[4]) + (matrix1[14] * matrix2[8]) + (matrix1[15] * matrix2[12]);
	result[13] = (matrix1[12] * matrix2[1]) + (matrix1[13] * matrix2[5]) + (matrix1[14] * matrix2[9]) + (matrix1[15] * matrix2[13]);
	result[14] = (matrix1[12] * matrix2[2]) + (matrix1[13] * matrix2[6]) + (matrix1[14] * matrix2[10]) + (matrix1[15] * matrix2[14]);
	result[15] = (matrix1[12] * matrix2[3]) + (matrix1[13] * matrix2[7]) + (matrix1[14] * matrix2[11]) + (matrix1[15] * matrix2[15]);
}


/* =========================================================================================== */
// Get Video Card info
/* =========================================================================================== */
void OpenGLClass::GetVideoCardInfo(char* gpu) {
	strcpy_s(gpu, 128, videoCardDesc);
}

