#version 130

#define MAX_DIST 100.0
#define MIN_DIST 0.01
#define MAX_STEP 100
#define M_PI 3.141592
in vec3 v_direction;

uniform vec3 u_camera_pos;

uniform vec3 u_light_pos;
uniform vec4 u_light_color;

vec4 color = vec4(0.8,.8,.8,1);

const vec4 sphere_color0 = vec4(1,0,0,1);
const vec3 sphere_pos0 = vec3(0.,0,10);
const float sphere_r0 = 5;


const vec4 plane_color0 = vec4(0,0,1,1);
const vec3 plane_pos0 = vec3(0,-5,0);
const vec3 plane_nor0 = vec3(0,1,0);

const vec4 torus_color0 = vec4(0,0.3,0.3,1);
const vec3 torus_pos0 = vec3(0,0,10);
const vec2 torus_r0 = vec2(3,1);

const vec4 box_color0 = vec4(0.7,0.7,0.7,1);
const vec3 box_pos0 = vec3(0,0,10);
const vec3 box_b0 = vec3(3,3,3);



out vec4 o_color;

float opSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float opSmoothSubtraction( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
    return mix( d2, -d1, h ) + k*h*(1.0-h); }

float opSmoothIntersection( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) + k*h*(1.0-h); }

float opUnion( float d1, float d2 ) { return min(d1,d2); }

float opSubtraction( float d1, float d2 ) { return max(-d1,d2); }

float opIntersection( float d1, float d2 ) { return max(d1,d2); }

float opDisplace(vec3 p ){
    const float k1 = 1;
    const float k2 = 2;
    const float k3 = 1;

    return sin(k1*p.x)*sin(k2*p.y)*sin(k3*p.z);
}

vec3 opTwist(vec3 p ){
    const float k = 0.1; // or some other amount
    float c = cos(k*p.y);
    float s = sin(k*p.y);
    mat2  m = mat2(c,-s,s,c);
    return vec3(m*p.xz,p.y);
}

vec3 opCheapBend( vec3 p ){
    const float k = 0.1; // or some other amount
    float c = cos(k*p.x);
    float s = sin(k*p.x);
    mat2  m = mat2(c,-s,s,c);
    return vec3(m*p.xy,p.z);
}

float sminCubic( float a, float b, float k ){
    float h = max( k-abs(a-b), 0.0 )/k;
    return min( a, b ) - h*h*h*k*(1.0/6.0);
}

float sdf_sphere(vec3 p, float r){
    return length(p) - r;
}


float sdf_torusXY( vec3 p, vec2 v){
    vec2 q = vec2(length(p.xy)-v.x,p.z);
    return length(q)-v.y;
}


float sdf_torusYZ( vec3 p, vec2 v){
    vec2 q = vec2(length(p.yz)-v.x,p.x);
    return length(q)-v.y;
}

float sdf_torusZX( vec3 p, vec2 v){
    vec2 q = vec2(length(p.xz)-v.x,p.y);
    return length(q)-v.y;
}

float SDF_world(vec3 p){
    float k = 0;
    float m = MAX_DIST;
    p = box_pos0 - p;
//    vec3 c = vec3(30,20,20);
//    p=mod(p+0.5*c,c)-0.5*c;

    //m = min(m,opSmoothUnion(sdf_sphere(p ,3),sdf_torusXY(vec3(7,0,0) + p,torus_r0),4));

    //float dis =opDisplace(p);
    //p = opCheapBend(p);
    //m = min(m,sdf_fractal1(p,sdf_box(p,vec3(3,3,3)),4));
    //    float d1 = sdf_torusXY(p,torus_r0);
    //    float d2 = sdf_torusYZ(p,torus_r0);
    //    float d3 = sdf_torusZX(p,torus_r0);

    //m = min(m,opSmoothUnion(d1,opSmoothUnion(d2,d3,.5),.5));
    vec3 dp = vec3(6,0,0);

    p+=dp;
    m = min(m,opSmoothUnion(sdf_torusXY(p,torus_r0/4),sdf_sphere(p,0.5),0.26));
    p-=dp;
    m = min(m,opSubtraction(sdf_sphere(p,2),sdf_torusXY(p,vec2(2,1))));
    p-=dp;
    m = min(m,opIntersection(sdf_sphere(p+vec3(0,0,1),2),sdf_torusZX(p,torus_r0)));

    return m;
}

vec4 len_way(vec3 dir,vec3 start){
    vec4 d = vec4(dir,1);
    vec4 s = vec4(start,0);
    for (int i=0;i<MAX_STEP;i++){
        float dist = SDF_world(s.xyz);
        s+= dist * d;
        if (dist < MIN_DIST) return s;
        if (s.w > MAX_DIST) {
            s.w = MAX_DIST;
            return s;
        }
    }
    return s;
}

vec3 calcul_normal(vec3 p){
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

    float gradient_x = SDF_world(p + small_step.xyy) - SDF_world(p - small_step.xyy);
    float gradient_y = SDF_world(p + small_step.yxy) - SDF_world(p - small_step.yxy);
    float gradient_z = SDF_world(p + small_step.yyx) - SDF_world(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}

vec4 get_color(vec3 p){

    vec4 objColor = color;
    float ambientStrength = 0.1;
    float specularStrength  = .5;
    float reflectionStrength = 1;

    vec3 N = calcul_normal(p);

    //    const vec3 small_step = vec3(0.001, 0.0, 0.0);
    //    const vec3 n = vec3(3,3,3);
    //
    //    float gradient_x = sdf_box(u_light_pos-p + small_step.xyy,n) - sdf_box(u_light_pos-p - small_step.xyy,n);
    //    float gradient_y = sdf_box(u_light_pos-p + small_step.yxy,n) - sdf_box(u_light_pos-p - small_step.yxy,n);
    //    float gradient_z = sdf_box(u_light_pos-p + small_step.yyx,n) - sdf_box(u_light_pos-p - small_step.yyx,n);

    //  vec3 L = normalize(vec3(gradient_x,gradient_y,gradient_z));
    vec3 L = normalize(u_light_pos - p);

    vec3 V = normalize(u_camera_pos-p);
    vec3 R = normalize(reflect(-L,N));
    vec3 T = normalize(2*dot(N,V)*N-V);

    float diff = max(dot(N,L),0.0);

    vec4 ambient = ambientStrength * u_light_color;
    vec4 diffuse = diff * u_light_color;
    vec4 specular = pow(max(dot(V,R),0.0),4) * specularStrength * u_light_color;
    vec4 reflectColor = diff * reflectionStrength * objColor;
    vec4 res = (ambient+diffuse+specular) * objColor;

    //    if (dot(N,L) > 0 ){
    //        vec4 shadow = len_way(L,2. * MIN_DIST * L+ p);
    //        if (shadow.w < MAX_DIST/2)
    //            res = res * color;
    //    }
    res[3] = .1;
    return res;
}

void main() {
    vec4 point = len_way(v_direction,u_camera_pos);
    if (point.w>=MAX_DIST)o_color = vec4(0.1,0.47,0.47,.1);
    else o_color = get_color(point.xyz);
}