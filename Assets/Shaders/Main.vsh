#version 450 compatibility

layout(location = 1) in float VertexAttrib;
layout(location = 2) in vec2 vTexCrood;
layout(location = 3) in vec3 vShade;
layout(location = 4) in vec3 vCrood;

out vOut {
	vec2 fTexCrood;
	vec3 fShade;
	vec3 fCrood;
	flat int facing;
};

void main() {
	facing = int(VertexAttrib + 0.5);
	fTexCrood = vTexCrood;
	fShade = vShade;
	fCrood = vCrood;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vCrood, 1.0);
}