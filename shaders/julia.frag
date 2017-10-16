#version 440

uniform highp vec4 color0;
uniform highp vec4 color1;
uniform highp vec4 color2;
uniform highp vec4 color3;
uniform highp vec4 color4;
uniform highp vec4 color5;
uniform highp vec4 color6;
uniform highp vec2 const_complex;
in vec2 txCoords;
uniform sampler1D rainbow_tx;
uniform bool is_julia;
uniform int iterations;
varying vec4 p;
const float threshold = 2.0;

void main( void )
{
	highp vec2 z = ( is_julia ) ? vec2( p.x, p.y ) : vec2( 0.0, 0.0 );
	highp vec2 c = ( is_julia ) ? const_complex : vec2( p.x, p.y );
	int count = -1;
	
	for ( int i = 0; i < iterations; i++ ) {
		highp vec2 nz = vec2( 0, 0 );
		nz.x = ( ( z.x * z.x ) - ( z.y * z.y ) ) + c.x;
		nz.y = ( 2.0 * ( z.x * z.y ) ) + c.y;
		if ( length( nz ) > threshold ) {
			count = i;
			break;
		}
		z = nz;
	}
	highp float k = log( float( count ) );
	
	if ( count < 0 )
		gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	else
		gl_FragColor = texture( rainbow_tx, k ).rgba;
}
