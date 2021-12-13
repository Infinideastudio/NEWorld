#version 450

layout(location=0) uniform sampler2D Tex;

in vOut {
	vec2 fTexCrood;
	vec3 fShade;
};
out vec4 fragment;

void main() {
	fragment = texture2D(Tex, fTexCrood) * vec4(fShade, 1.0);
}