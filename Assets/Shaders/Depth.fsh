#version 450
layout(location = 0) out vec4 color;
void main() {
	float shade = (gl_FragCoord.z / gl_FragCoord.w + 1.0) * 0.5;
	color = vec4(shade, shade, shade, 1.0);
}
