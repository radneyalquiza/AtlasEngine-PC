// Atlas Graphics Implementation

#include "AtlasGraphics.h"

// default constructor
AtlasGraphics::AtlasGraphics() {
	opengl = 0;
}

// copy
AtlasGraphics::AtlasGraphics(const AtlasGraphics& src) {

}

// destructor
AtlasGraphics::~AtlasGraphics() {

}


/* ================================================================================== */
// INITIALIZE
/* ================================================================================== */
bool AtlasGraphics::initialize(OpenGLClass* ogl, HWND hwnd) {
	// store pointer to the opengl object
	opengl = ogl;

	return true;
}

/* ================================================================================== */
// shutdown
/* ================================================================================== */
void AtlasGraphics::shutdown() {

	// release opengl pointer
	opengl = 0;
}

/* ================================================================================== */
// Frame
/* ================================================================================== */
bool AtlasGraphics::Frame() {
	bool result;

	// attempt to render the graphics scene
	result = render();
	if(!result)
		return false;

	return true;
}


/* ================================================================================== */
// Render
/* ================================================================================== */
bool AtlasGraphics::render() {

	// call helper functions to build the scene to the back buffer
	opengl->beginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// flip the back buffer
	opengl->endScene();

	return true;
}