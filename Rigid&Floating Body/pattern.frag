#version 120

uniform float    uTime;        // "Time", from Animate( )
varying vec2      vST;        // texture coords

uniform float uS0, uT0;
uniform float uSize;

const float PI =     3.14159265;

void
main( )
{
    vec3 myColor = vec3( 0,0,1 );

    if( uS0-uSize/(PI/2) <= vST.s && vST.s <= uS0+uSize/(PI/2) &&
        uT0-uSize/PI <= vST.t && vST.t <= uT0+uSize/PI)
    {
        myColor = vec3( 0., 1., 0.); //  frag is green
    }

    gl_FragColor = vec4( myColor,  1. );
}
