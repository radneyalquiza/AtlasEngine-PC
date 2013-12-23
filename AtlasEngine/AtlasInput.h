// Atlas Input Declaration

#ifndef _ATLASINPUT_H_
#define _ATLASINPUT_H_

class AtlasInput {
public:
	AtlasInput();
	AtlasInput(const AtlasInput&);
	~AtlasInput();

	void initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool isKeyDown(unsigned int);

private:
	bool keys[256];
};

#endif