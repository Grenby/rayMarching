#version 130

in vec2 v_pos;

uniform vec2 u_c;

out vec4 o_color;

vec2 cMultiply(vec2 v1,vec2 v2){
    return vec2(v1.x*v2.x - v1.y*v2.y,v1.x*v2.y + v1.y*v2.x);
}

//vec2 pow(vec2 v,float p){
//    float l = length(v);
//    float angle = atan(v.y,v.x);
//
//    angle = angle * p;
//    l = pow(l,p);
//    return vec2(l*cos(angle),l*sin(angle));
//}

void main() {
    vec2 v = v_pos / 400 ;
    const float R = 100;

    int iter =0;
    int max_iter = 100;

    while(v.x*v.x+v.y*v.y < R && iter < max_iter){
        v = cMultiply(v, v) + u_c;
        iter++;
    }

    float p = 1 - iter/max_iter;
    float r = 0.4 + 0.25*sin( 0.5 + p);
    float g = 0.6 + 0.25*sin( 0.9 + p);
    float b = 0.7 + 0.25*sin( 1.3 + p);

    o_color = vec4(r*p,g*p,b*p, 1);
}
