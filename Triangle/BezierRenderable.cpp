#include "BezierRenderable.h"



BezierRenderable::BezierRenderable()
{
	setDirty(VTX_DIRTY);
}


BezierRenderable::~BezierRenderable()
{
}

void BezierRenderable::render(const MVP& mvp)
{
	myUpdateBuffers();
	auto program = myProgram.lock();
	if (!program)
		return;
	program->use();
	program->setUniform(mvp.get(), "MVP");
	program->setUniform(1, "NumStrips");
	program->setUniform(30, "NumSegments");
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindVertexArray(myVAO);
	glDrawArrays(GL_PATCHES, 0, 4);
}

void BezierRenderable::myUpdateBuffers()
{
	if (!isDirty(VTX_DIRTY))
		return;
	glGenVertexArrays(1, &myVAO);
	glBindVertexArray(myVAO);

	glGenBuffers(1, &myVBO);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), PositionData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	removeDirty(VTX_DIRTY);
}