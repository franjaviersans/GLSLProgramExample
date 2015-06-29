#version 450 core

uniform sampler2D mtex;

in VertexData{
	vec2 vTexCoord;
} VertexIn;

layout(location = 0) out vec4 vFragColor;

void main(void)
{
	vFragColor = texture(mtex, VertexIn.vTexCoord);
}