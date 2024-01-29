#version 440
#define PI 3.14159265358
out vec4 vs_color;
// We need the frame size
uniform float frameSizeX; 
uniform float frameSizeY;

float plot(vec2 st){

    return smoothstep(0.02,0.0,abs(st.y-st.x));
}


void main(){
    vec2 st;
    st.x = gl_FragCoord.x/frameSizeX; //Go to 0-1
    st.y = gl_FragCoord.y/frameSizeY;

    //float y = step(0.5,st.x); //
    //float y = smoothstep(0.1,0.9,st.x);
    float y = smoothstep(0.2,0.5,st.x) - smoothstep(0.5,0.8,st.x);
    vec3 color =  vec3(y);

    //plot a line
    //float pct = plot(st);
    //color = (1.0-pct)*color+pct*vec3(0.0,1.0,0.0);

    vs_color = vec4(color,1.0);
    
    
    }