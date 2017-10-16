#version 440

layout ( location = 0 ) in vec4 pos;
layout ( location = 1 ) in vec2 uv;
uniform float aspect;
uniform vec4 center;
varying vec4 p;
out vec2 txCoords;

void main( void )
{
	gl_Position = pos * gl_ModelViewProjectionMatrix;
	p = ( pos + center ) / aspect * vec4( aspect, 1.0, 1.0, 1.0 );
	txCoords = uv; 
}
