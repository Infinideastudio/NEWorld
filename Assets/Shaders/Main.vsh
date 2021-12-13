#version 450 compatibility

in float VertexAttrib;
out vec4 vertex;
flat out int facing;

void main() {
	facing = int(VertexAttrib + 0.5);
	vertex = gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}