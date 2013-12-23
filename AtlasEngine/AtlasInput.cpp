// Atlas Input Implementation

#include "AtlasInput.h"

// default constructor
AtlasInput::AtlasInput() {

}

// copy
AtlasInput::AtlasInput(const AtlasInput& src) {

}

// destructor
AtlasInput::~AtlasInput() {

}

/* ================================================================================== */
// INITIALIZE
/* ================================================================================== */
void AtlasInput::initialize() {

	int i;

	// initialize all keys to being released and not pressed
	for(i=0; i < 256; i++)
		keys[i] = false;

}

/* ================================================================================== */
// Keydown
/* ================================================================================== */
void AtlasInput::KeyDown(unsigned int input) {
	keys[input] = true;
}

/* ================================================================================== */
// Keyup
/* ================================================================================== */
void AtlasInput::KeyUp(unsigned int input) {
	keys[input] = false;
}

/* ================================================================================== */
// is key pressed
/* ================================================================================== */
bool AtlasInput::isKeyDown(unsigned int key) {
	return keys[key];
}