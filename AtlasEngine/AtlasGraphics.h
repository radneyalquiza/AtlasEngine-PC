// Atlas Graphics Declaration

#ifndef _ATLASGRAPHICS_H_
#define _ATLASGRAPHICS_H_

#include "OpenGLClass.h"

// GLOBALS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

// AtlasGraphics Class
class AtlasGraphics {

public:
	AtlasGraphics();
	AtlasGraphics(const AtlasGraphics&);
	~AtlasGraphics();

	bool initialize(OpenGLClass*, HWND);
	void shutdown();
	bool Frame();

private:
	bool render();

private:
	OpenGLClass* opengl;
};


#endif