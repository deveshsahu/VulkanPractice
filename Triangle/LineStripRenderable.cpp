#include "LineStripRenderable.h"



LineStripRenderable::LineStripRenderable()
{
	setDirty(VTX_DIRTY);
}


LineStripRenderable::~LineStripRenderable()
{
}

void LineStripRenderable::render(const MVP& mvp)
{
	auto program = myProgram.lock();
	if (!program)
	{
		return;
	}
	program->use();
	myUpdateBuffer();
	glBindVertexArray(myVAO);
	glDrawArrays(GL_LINE_STRIP, 0, 4);
}

void LineStripRenderable::myUpdateBuffer()
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