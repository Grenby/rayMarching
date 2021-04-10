#version 120

in vec2 i_pos;
in vec2 i_tex_coords;

out vec2 v_tex_coords;

void main() {
    gl_Position = vec4(i_pos,0,1);
    v_tex_coords = i_tex_coords;
}
