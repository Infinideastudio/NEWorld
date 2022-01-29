#version 450

layout(location=0) uniform sampler2D Tex;

in vOut {
    vec2 fTexCrood;
    flat vec2 fShade;
};
out vec4 fragment;

void main() {
    fragment = texture(Tex, fTexCrood) * vec4(vec3(fShade.x), fShade.y);
}