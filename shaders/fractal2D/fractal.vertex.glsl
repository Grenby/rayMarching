#version 130

in vec2 i_pos;

uniform vec2 u_window;

out vec2 v_pos;

void main() {
    vec2 pos = i_pos - 0.5 * u_window;
    v_pos = pos;
    pos/=u_window;
    pos*=2;
    gl_Position = vec4(pos, 0 ,1);
}
