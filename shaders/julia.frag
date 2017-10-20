#version 440

precision highp float;

uniform vec2 const_complex;
uniform sampler1D rainbow_tx;
uniform bool is_julia;
uniform int iterations;
varying vec4 p;
const vec4 BLACK = vec4( 0.0, 0.0, 0.0, 1.0 );

#define L_SHF 0
#define R_SHF 1

void main( void )
{
	vec2 z = ( is_julia ) ? p.xy : vec2( 0.0, 0.0 );
	vec2 c = ( is_julia ) ? const_complex : p.xy;
	
	float n = 0.0;
	float p_sq = p.y * p.y;
	float x_off = p.x - 0.25;
	float qx = ( x_off * x_off ) + p_sq;
	
	if ( qx * ( qx + x_off ) < p_sq / 4 ) {
		gl_FragColor = BLACK;
	} else {
		for ( int i = 0; i < iterations; i++ ) {
			z = vec2( ( z.x * z.x ) - ( z.y * z.y ), 2 * ( z.x * z.y ) ) + c;
			if ( dot( z, z ) > ( iterations * iterations ) ) break;
			n += 1.0;
		}
		
		if ( n == iterations ) {
			gl_FragColor = BLACK;
		} else {/*
			float rainbow_k = ( round( texture1D( rainbow_tx, 1.0 ).r * 255 ) << 24 ) +
				( round( texture1D( rainbow_tx, 1.0 ).g * 255 ) << 16 ) +
				( round( texture1D( rainbow_tx, 1.0 ).b * 255 ) << 8 ) +
				round( texture1D( rainbow_tx, 1.0 ).a * 255 );
			float mu = n - log( log( ( z.x * z.x ) + ( z.y * z.y ) ) ) / log( 2.0 );
			float z_sin = float( sin( mu / rainbow_k ) ) / 2 + 0.5;
			float z_cos = float( cos( mu / rainbow_k ) ) / 2 + 0.5;
			gl_FragColor = vec4( z_cos, z_cos, z_sin, 1.0 ); 
		*/
			gl_FragColor = mix( vec4( 0.55, 0.55, 0.475, 0.1 ), vec4( 0.5, 0.125, 0.7, 0.1 ), n / 100.0 ).rgba;
		}
	}
}
