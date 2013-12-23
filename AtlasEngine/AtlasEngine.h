// ATLAS ENGINE

// this header defines the Atlas Engine

#ifndef _ATLASENGINE_H_
#define _ATLASENGINE_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

// include engine components
#include "OpenGLClass.h"
#include "AtlasInput.h"
#include "AtlasGraphics.h"

// The engine class declaration
class AtlasEngine
{
public:
	AtlasEngine();
	AtlasEngine(const AtlasEngine&);
	~AtlasEngine();

	bool initialize();
	void shutdown();
	void run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	bool initializeWindows(OpenGLClass*, int&, int&);
	void shutdownWindows();

private:
	LPCWSTR appName;
	HINSTANCE hinstance;
	HWND hwnd;

	OpenGLClass* opengl;
	AtlasInput* input;
	AtlasGraphics* graphics;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static AtlasEngine* ApplicationHandle = 0;

#endif