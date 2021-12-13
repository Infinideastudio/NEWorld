#version 450 compatibility
layout(location = 4) in vec3 vCrood;
void main() {
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vCrood, 1.0);
}