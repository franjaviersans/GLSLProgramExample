#version 450 core

uniform mat4 mProjection, mModelView;

layout(location = 0) in vec3 vVertex;
layout(location = 3) in vec2 vTexCoord;


out VertexData{
	vec2 vTexCoord;
} VertexOut;

void main()
{
	VertexOut.vTexCoord = vTexCoord;
	gl_Position = mProjection * mModelView * vec4(vVertex,1.0f);
}