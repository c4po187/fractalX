#version 440

precision highp float;

uniform vec2 const_complex;
uniform sampler1D rainbow_tx;
uniform bool is_julia;
uniform int iterations;
varying vec4 p;
const vec4 BLACK = vec4( 0.0, 0.0, 0.0, 1.0 );

void main( void )
{
	vec2 z = ( is_julia ) ? p.xy : vec2( 0.0, 0.0 );
	vec2 c = ( is_julia ) ? const_complex : p.xy;
	int i = 0;
	float n = 0.0;
	
	for ( ; i < iterations; i++ ) {
		vec2 _z = z;
		z = vec2( ( z.x * z.x ) - ( z.y * z.y ), 2 * ( z.x * z.y ) ) + c;
		if ( dot( z, z ) > ( iterations * iterations ) ) break;
		n = n + exp( -sqrt( length( z ) ) - 0.5 / ( sqrt( length( _z - z ) ) ) );
	}
		
	if ( i == iterations ) {
		gl_FragColor = BLACK;
	} else { 
		gl_FragColor = texture1D( rainbow_tx, n ).rgba;
	}
}
