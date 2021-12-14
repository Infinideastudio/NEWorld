#version 450 compatibility

layout(location = 2) in vec2 vTexCrood;
layout(location = 3) in vec3 vShade;
layout(location = 4) in vec3 vCrood;

out vOut {
	vec2 fTexCrood;
	vec3 fShade;
};

void main() {
	fTexCrood = vTexCrood;
	fShade = vShade;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vCrood, 1.0);
}