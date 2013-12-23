// ATLAS ENGINE IMPLEMENTATION

#include "AtlasEngine.h"

/* ================================================================================== */
// default constructor
/* ================================================================================== */
AtlasEngine::AtlasEngine() {
	opengl = 0;
	input = 0;
	graphics = 0;
}

/* ================================================================================== */
// copy
/* ================================================================================== */
AtlasEngine::AtlasEngine(const AtlasEngine& src) {

}
/* ================================================================================== */
// destructor
/* ================================================================================== */
AtlasEngine::~AtlasEngine() {

}

/* ================================================================================== */
// INITIALIZE
/* ================================================================================== */
bool AtlasEngine::initialize() {
	
	int swidth, sheight;
	bool result;

	// init dimensions
	swidth = 0;
	sheight = 0;

	// create opengl
	opengl = new OpenGLClass;
	if(!opengl)
		return false;

	// create a window
	result = initializeWindows(opengl, swidth, sheight);
	if(!result)
	{
		MessageBox(hwnd, L"Failed to initialize the window.", L"Error", MB_OK);
		return false;
	}

	input = new AtlasInput;
	if(!input)
		return false;

	// no need to return a status for input class initialization
	input->initialize();

	graphics = new AtlasGraphics;
	if(!graphics)
		return false;

	result = graphics->initialize(opengl, hwnd);
	if(!result)
		return false;

	return true;
}

/* ================================================================================== */
// cleanup
/* ================================================================================== */
void AtlasEngine::shutdown() {

	// release the graphics object
	if(graphics) {
		graphics->shutdown();
		delete graphics;
		graphics = 0;
	}

	// release the input object
	if(input) {
		delete input;
		input = 0;
	}

	// release the opengl object
	if(opengl) {
		delete opengl;
		opengl = 0;
	}

	// shutdown window
	shutdownWindows();
}

/* ================================================================================== */
// the main run loop
/* ================================================================================== */
void AtlasEngine::run() {

	MSG msg;
	bool done, result;

	// initialize message structure
	ZeroMemory(&msg, sizeof(MSG));

	// loop until the close is initiated
	done = false;
	while(!done)
	{
		// handle windows message
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// if windows signals to end the application
		if(msg.message == WM_QUIT) 
			done = true;
		else
		{
			result = Frame();
			if(!result)
				done = true;
		}

	}
}

/* ================================================================================== */
// FRAME Constructor
/* ================================================================================== */
bool AtlasEngine::Frame() {

	bool result;

	// check if the escape button is pressed
	if(input->isKeyDown(VK_ESCAPE))
		return false;

	// do frame processing
	result = graphics->Frame();
	if(!result)
		return false;

	return true;
}

/* ================================================================================== */
// Handler for Messages
/* ================================================================================== */
LRESULT CALLBACK AtlasEngine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	
	switch(umsg)
	{
		// check for normal key
	case WM_KEYDOWN:
		{
			// if key is pressed, send to input object so it can be registered
			input->KeyDown((unsigned int)wparam);
			return 0;
		}
	case WM_KEYUP:
		{
			// if key is released, send to input object so you can unset the state for that key
			input->KeyUp((unsigned int)wparam);
			return 0;
		}
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

/* ================================================================================== */
// Initialize window for the engine
/* ================================================================================== */
bool AtlasEngine::initializeWindows(OpenGLClass* ogl, int& width, int& height) {

	WNDCLASSEX wc;
	DEVMODE dmscreensettings;
	int posX, posY;

	// get external pointer to this object
	ApplicationHandle = this;

	// get instance of the current application
	hinstance = GetModuleHandle(NULL);

	// give the application name
	appName = L"Atlas Engine";

	// setup the windows class
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// register the window class
	RegisterClassEx(&wc);

	// create temporary window for OPENGL
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName, WS_POPUP,
		0, 0, 800, 600, NULL, NULL, hinstance, NULL);
	if(hwnd == NULL)
		return false;

	// dont show the window
	ShowWindow(hwnd, SW_HIDE);

	// release the temporary window
	DestroyWindow(hwnd);
	hwnd = NULL;

	// determine resolution of the client's desktop screen
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	// setup the screen settings depending if it is on Fullscreen/Windowed mode
	if(FULL_SCREEN)
	{
		// if fulscreen, set the screen to maximum size
		memset(&dmscreensettings, 0, sizeof(dmscreensettings));
		dmscreensettings.dmSize = sizeof(dmscreensettings);
		dmscreensettings.dmPelsWidth = (unsigned long)width;
		dmscreensettings.dmPelsHeight = (unsigned long)height;
		dmscreensettings.dmBitsPerPel = 32;
		dmscreensettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// change display settings to full screen
		ChangeDisplaySettings(&dmscreensettings, CDS_FULLSCREEN);
		// set position of window
		posX = posY = 0;
	}
	else
	{
		// windowed dimensions
		width = 800;
		height = 600;

		// place window in the middle of screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - width)/2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height)/2;
	}

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName, WS_POPUP,
		posX, posY, width, height, NULL, NULL, hinstance, NULL);

	if(hwnd == NULL)
		return false;

	// bring the window up on the screen and focus
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// hide the mouse
	ShowCursor(false);

	return true;
}

/* ================================================================================== */
// Shutdown window
/* ================================================================================== */
void AtlasEngine::shutdownWindows() {

	// show the mouse
	ShowCursor(true);

	// fix the display settings
	if(FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	/// remove the window
	DestroyWindow(hwnd);
	hwnd = NULL;

	// remove the application instance
	UnregisterClass(appName, hinstance);
	hinstance = NULL;

	// release the pointer to the class
	ApplicationHandle = NULL;
}

/* ================================================================================== */
// Windows process
/* ================================================================================== */
LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {

	switch(umsg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	default:
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}