#pragma once
#include "BaseRenderable.h"

#include "gl_core_4_5.h"
#include <glm\glm.hpp>

#include <vector>


class LineStripRenderable :
	public BaseRenderable
{
public:
	LineStripRenderable();
	~LineStripRenderable();

	void render(const MVP& mvp) override;

protected:

	void myUpdateBuffer();

	std::vector<glm::vec3> PositionData =
	{
		glm::vec3(-0.5, 0.0, 0.0),
		glm::vec3(0.5, 0.0, 0.0),
		glm::vec3(0.0, 0.5, 0.0)
	};

	GLuint myVAO;
	GLuint myVBO;
};

