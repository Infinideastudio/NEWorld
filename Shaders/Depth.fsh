#version 110
varying vec4 pos;
void main() {
    float color = (pos.z / pos.w + 1.0) * 0.5;
    gl_FragColor = vec4(color, color, color, 1.0);
}