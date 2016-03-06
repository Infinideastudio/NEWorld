#version 110

attribute float VertexAttrib;
varying vec4 VertCoords;
varying float facing_float;

void main() {
	facing_float = VertexAttrib;
	VertCoords = gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}