#version 450

layout (location = 0) in vec3 inVertex;

layout (location = 0) uniform mat4 MVP;
out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = mvp * vec4(inVertex, 1.0f);
}