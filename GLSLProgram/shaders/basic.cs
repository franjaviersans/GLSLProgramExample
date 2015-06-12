#version 450

uniform mat4 mProjection, mModelView;

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vColor;

out vec3 vVertexColor;

void main()
{
	vVertexColor = vColor;
	gl_Position = mProjection * mModelView * vec4(vVertex,1.0f);
}