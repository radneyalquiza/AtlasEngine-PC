// CLIENT PROGRAM
// This program will be used by client to access the AtlasEngine functionality

#include "AtlasEngine.h"

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, PSTR pscmdline, int icmdshow)
{
	AtlasEngine* engine;
	bool result;

	// create the system object
	engine = new AtlasEngine;
	if(!engine)
		return 0;

	// initialize and run the engine
	result = engine->initialize();
	if(result)
		engine->run();

	// shutdonw and release the engine
	engine->shutdown();
	delete engine;
	engine = 0;

	return 0;
}