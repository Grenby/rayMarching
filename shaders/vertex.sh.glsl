#version 130
in vec3 i_direction;
in vec2 i_pos;

uniform mat3 transfrom;

out vec3 v_direction;

void main() {
    v_direction = transfrom * i_direction;
    gl_Position = vec4(i_pos,0,1);
}