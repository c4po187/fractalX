#version 440

uniform float aspect;
uniform vec4 center;

varying vec4 p;

void main( void )
{
	gl_Position = gl_Vertex * gl_ModelViewProjectionMatrix;
	p = ( gl_Vertex + center ) / aspect * vec4( aspect, 1.0, 1.0, 1.0 ); 
}
