#pragma once
#include "GLSLProgram.h"
#include "Util.h"
#include <memory>

class BaseRenderable
{
public:
	enum
	{
		VTX_DIRTY = 1 << 0,
		SIZE
	};

	BaseRenderable() = default;
	virtual ~BaseRenderable() = default;

	virtual void render(const MVP& mvp) = 0;
	void setDirty(int flag) { myDirtyFlag |= flag; }
	void removeDirty(int flag) { myDirtyFlag &= ~flag; }
	bool isDirty(int flag) { return myDirtyFlag & flag; }
	void addProgram(std::weak_ptr<GLSLProgram> prg) { myProgram = prg; }
protected:
	int myDirtyFlag = 0x00;
	std::weak_ptr<GLSLProgram> myProgram;
};