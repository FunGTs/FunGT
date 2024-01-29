#version 440
#define PI 3.14159265358
out vec4 vs_color;
uniform float frameSizeX; 
uniform float frameSizeY;
uniform float time; 
const vec3 yellow = vec3(1.000, 0.843, 0.000);
const vec3 blue = vec3(0.000,0.0,1.0);
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
struct Sphere{
    vec3 center; 
    float radii; 
};
struct Plane{
    vec3 normal; 
    vec3 point; //Some random point such that for all points p in the plane dot((p-point),normal)==0; 
};
vec3 stripe(vec3 point){
     float d;
    if (abs(point.y) > 0.55) {
      // smooth the stripe
         d = abs(point.y);
         d = smoothstep(0.2, 0.36, d);
        return mix(blue, yellow, d);
    }

}
void AnimateSphere(inout Sphere sphere) {
    //Size of the animation loop
	float loopRadius = 1.0;
    //initial sphere position
    float loopOffset = sphere.center.z;
    //time it takes to complete a loop
    float loopTime = 10.0;
    
    //Based on parametric equations of a circle
     sphere.center.y = sin((mod(time, loopTime))/loopTime*2.0*PI)*loopRadius;
     //sphere.center.z = loopOffset + cos((mod(time,loopTime))/loopTime*2.0*PI)*loopRadius;
    
}
void animate(vec3 obj){
    float loopRadius = 1.0;
    //initial sphere position
    float loopOffset = 1.0;
    //time it takes to complete a loop
    float loopTime = 10.0;


    //Based on parametric equations of a circle
     obj.xy.y = sin((mod(time, loopTime))/loopTime*2.0*PI)*loopRadius;
     //obj.z = loopOffset + cos((mod(time,loopTime))/loopTime*2.0*PI)*loopRadius;
}
    
void main(){

    //Get the coordinates in the range [0,1]: 
    vec2 coord;
     float loopTime = 10.0;
     float loopRadius = 1.0;
    //float scale = 1. * ((1.01 + sin(.5 * time)) * 6.);
    float scale = sin((mod(time, loopTime))/loopTime*2.0*PI)*loopRadius;
    coord.x = (gl_FragCoord.x /frameSizeX);
    coord.y = (gl_FragCoord.y /frameSizeY);
    //Convert from [0,1] to [-1,1]
    //This moves the pixel (0,0) to the middle of of the screen
    coord.x = coord.x*2.0 - 1; 
    coord.y = coord.y*2.0 - 1 + scale;
    
    //This is necessary if the canvas is not a Square 
    float aspect = frameSizeX/frameSizeY; 
    coord.x = coord.x*aspect; 
    vec3 pixelPos = vec3(coord.x,coord.y,0);
    vec3 cameraPos = vec3(0.0, 0.0, -4.0); 
    vs_color = vec4(0.0,0.0,0.0,1.0); //background is black; 
    //Put a sphere at the orgin:
    Sphere mySphere = Sphere(vec3(0.f,0.f,10.f),0.8);
    
    //AnimateSphere(mySphere);   
  
 
    float dis = 1.0-length(pixelPos.xy-mySphere.center.xy);
   

    float fade = 0.01;
    float star = sdStar5(pixelPos.xy,0.12,0.4);
    vec3 starObj = vec3(smoothstep(0.f,fade,-star));
    //animate(mySphere.center);



    dis = step(mySphere.radii,dis);
  
  
    vec3 ball1 = yellow*vec3(dis);


    vec3 colorStar = starObj*starBaseColor; 
    animate(colorStar);
    vec3 color = mix(ball1,colorStar,smoothstep(0.f,fade,-star));
   
    vs_color = vec4(color,1.0);
    // if(dis<mySphere.radii)
    // {

    //     //dis = 1.0;
    //     //vs_color = vec4(vec3(1.0,0,0,0.0),1.0);
    //     vs_color = vec4(0.0,1.0,0.0,1.0);
    // }
   

    
   


    
}