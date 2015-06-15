#version 450 core

in VertexData{
	vec3 vColor;
} VertexIn;

layout(location = 0) out vec4 vFragColor;

void main(void)
{
	vFragColor = vec4(VertexIn.vColor,1.0f);
}