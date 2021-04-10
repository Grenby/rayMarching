#version 130
in vec3 i_position;
in vec3 i_color;

out vec4 v_color;

uniform mat4 u_projection_matrix;

void main(){
    gl_Position =  vec4(i_position, 1.0);
    v_color = vec4(i_color,.3);
}