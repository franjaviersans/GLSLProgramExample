#version 450


layout(triangles, invocations = 1) in;
layout (line_strip, max_vertices = 4) out;

in VertexData{
	vec3 vColor;
} VertexIn[];


out VertexData{
	vec3 vColor;
} VertexOut;

 
void main()
{

	// copy attributes
	gl_Position = gl_in[0].gl_Position;
	VertexOut.vColor = VertexIn[0].vColor;
	// done with the vertex
	EmitVertex();

	// copy attributes
	gl_Position = gl_in[1].gl_Position;
	VertexOut.vColor = VertexIn[1].vColor;
	// done with the vertex
	EmitVertex();

	// copy attributes
	gl_Position = gl_in[2].gl_Position;
	VertexOut.vColor = VertexIn[2].vColor;
	// done with the vertex
	EmitVertex();

	// copy attributes
	gl_Position = gl_in[0].gl_Position;
	VertexOut.vColor = VertexIn[0].vColor;
	// done with the vertex
	EmitVertex();

	EndPrimitive();
}