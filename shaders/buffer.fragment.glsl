#version 120

in vec2 v_tex_coods;

uniform sampler2D screenTexture;

void main() {
    FragColor = texture(screenTexture,v_tex_coods);
}
