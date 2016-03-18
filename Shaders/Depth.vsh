#version 110
varying vec4 pos;
void main() {
    pos = ftransform();
    gl_Position = pos;
}