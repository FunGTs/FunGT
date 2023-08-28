#version 440

uniform float frameSizeX;
uniform float frameSizeY;
uniform vec2 mouseInput;
in vec2 poinPos; 
out vec4 vs_color;
const vec3 baseColor = vec3(0.6,0.5,0.0);
const vec3 yellow = vec3(1.000, 0.843, 0.000);
const vec3 blue = vec3(0.000,0.0,1.0);
const vec3 stripeColor = vec3(0.0,0.300,0.600);
const vec3 white  = vec3(1.0);

vec3 colorLookup(vec3 point, int number) {
// the cue ball is white
  if (number == 0)
    return baseColor;
  // all other balls have a zero based index
  number--;
  float d;
  // use a fixed color of yellow for now
  vec3 color = stripeColor; 
  // if we are in a striped ball
  if (number > 7)
    if (abs(point.y) > 0.55) {
      // smooth the stripe
      d = abs(point.y);
      d = smoothstep(0.35, 0.36, d);
      return mix(blue, yellow, d);
    }
//   // generate the circle for the number area
//   d = distance(point.xy, vec2(0));
//   // smooth the circle
//   if (d < 0.4) return white;
//   d = smoothstep(0.4, 0.41, d);
//   return mix(white, color, d);
} 
void main() {
    // // convert resolution to coordinates int the 0..1 range
    vec2 coord; 
    coord.x = gl_FragCoord.x/ frameSizeX;
    coord.y = gl_FragCoord.y/ frameSizeY;
    // // use the center to create a circle mask
    vec2 center = vec2(0.5,0.5);
    float d = distance(center, coord) / 0.5;
    float a = d > 0.99 ? 1.0 - smoothstep(0.99, 1.0, d) : 1.0;
   // create uv coords in the -1..1 range
    vec2 uv = ( 2.0*coord - 1.0);
        // build our normal using xy and a calculated z
    vec3 n = vec3(uv, sqrt(1.0 - clamp(dot(uv, uv), 0.0, 1.0)));
    // generate a surface normal map
    // create a primary light source
    vec3 light = vec3(-mouseInput.x, mouseInput.y *0.5, 400.0);
    // normalize the light as a direction
    light = normalize(light);
    // use dot product to find the brightness of the pixel on the sphere
    float brightness = clamp(dot(light, n), 0.1, 1.0);
    vec3 diffuse = colorLookup(n, 9) * brightness;
    vs_color = vec4(diffuse, a);
}