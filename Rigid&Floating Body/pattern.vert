#version 120

uniform float    uTime;        // "Time", from Animate( )
varying vec2      vST;        // texture coords

const float PI =     3.14159265;
const float AMP =     0.2;        // amplitude
const float W =     2.;        // frequency

//const vec3 LIGHTPOSITION = vec3( 5., 5., 0. );

void
main( )
{   
    vST = gl_MultiTexCoord0.st;
    vec3 vert = gl_Vertex.xyz;

    vert.x = vert.x * (1. + sin(6.* W * PI*(uTime - 0.1)) * AMP * 4);
  

    vert.y = vert.y * (2. + sin(6.* W * PI*(uTime))) * AMP * 4;
  

    vert.z = vert.z * (1. + sin(6.* W * PI*(uTime + 0.1)) * AMP * 4);

    
    gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );
}
