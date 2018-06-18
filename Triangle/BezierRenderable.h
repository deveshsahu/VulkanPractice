#pragma once
#include "BaseRenderable.h"
class BezierRenderable :
	public BaseRenderable
{
public:
	BezierRenderable();
	~BezierRenderable();

	void render(const MVP& mvp) override;
};

