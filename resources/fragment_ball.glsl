#version 440
#define PI 3.14159265358
out vec4 vs_color;
uniform float frameSizeX; 
uniform float frameSizeY;
uniform float time; 
const vec3 baseColor = vec3(0.6, 0.5, 0.0); 
const vec3 starBaseColor = vec3(0.6,0.0,0.0);
float sdStar5(vec2 p, float r, float rf)
{
  const vec2 k1 = vec2(0.809016994375, -0.587785252292);
  const vec2 k2 = vec2(-k1.x,k1.y);
  p.x = abs(p.x);
  p -= 2.0*max(dot(k1,p),0.0)*k1;
  p -= 2.0*max(dot(k2,p),0.0)*k2;
  p.x = abs(p.x);
  p.y -= r;
  vec2 ba = rf*vec2(-k1.y,k1.x) - vec2(0,1);
  float h = clamp( dot(p,ba)/dot(ba,ba), 0.0, r );

  return length(p-ba*h) * sign(p.y*ba.x-p.x*ba.y);
}
void main() {
    // convert resolution to coordinates int the -1,1 range
    vec2 coord;
    float scale  = 1; /*= 1. * ((1.01 + sin(.5 * time)) * 6.);*/
    coord.x = (gl_FragCoord.x /frameSizeX);
    coord.y = (gl_FragCoord.y /frameSizeY);
    //Convert from [0,1] to [-1,1]
    //This moves the pixel (0,0) to the middle of of the screen
    coord.x = scale*coord.x*2.0 - 1; 
    coord.y = scale*coord.y*2.0 - 1;
    float aspect = frameSizeX/frameSizeY; 
    coord.x = coord.x*aspect; 
    // coord.x+=pos.x; 
    // coord.y+=pos.y;
    float fade = 0.01;
    // use the center to create a circle mask
    vs_color = vec4(0.0, 0.0, 1.0, 1.0);
    float star = sdStar5(coord,0.6,0.4);
    vec3 starObj = vec3(smoothstep(0.f,fade,-star));
    float d = 1.0-sqrt(dot(coord,coord)); 
    //Normal: 
    


    vec3 sphere = vec3(smoothstep(0.0,fade,d));
    vec3 colorBall = baseColor*sphere;
    vec3 colorStar = starObj*starBaseColor; 
    vec3 color = mix(colorBall,colorStar,smoothstep(0.f,fade,-star));
    vs_color = vec4(color,1.0);

}