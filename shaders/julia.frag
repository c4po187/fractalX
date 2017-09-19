#version 440

uniform vec4 color0;
uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;
uniform vec4 color4;
uniform vec4 color5;
uniform vec4 color6;
uniform vec2 const_complex;
uniform bool is_julia;
uniform int iterations;

varying vec4 p;

const float threshold = 2.0;

void main( void )
{
	vec2 z = ( is_julia ) ? vec2( p.x, p.y ) : vec2( 0.0, 0.0 );
	vec2 c = ( is_julia ) ? const_complex : vec2( p.x, p.y );
	int count = -1;
	
	for ( int i = 0; i < iterations; i++ ) {
		vec2 nz = vec2( 0, 0 );
		nz.x = ( ( z.x * z.x ) - ( z.y * z.y ) ) + c.x;
		nz.y = ( 2.0 * ( z.x * z.y ) ) + c.y;
		if ( length( nz ) > threshold ) {
			count = i;
			break;
		}
		z = nz;
	}
	
	float k = log( 0.57713 / sqrt( float( count ) / 100 ) );
	
	if ( count == 0 )
		gl_FragColor = vec4( 0.1, 0.2, 0.3, 1.0 );
	else if ( count == -1 )
		gl_FragColor = vec4( 0, 0, 0, 0 );
	else {
		gl_FragColor = ( color4 * k ) + ( color1 / log( 1 - k ) );
	}
}
