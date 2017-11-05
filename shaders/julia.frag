#version 440

precision highp float;

uniform vec2 const_complex;
uniform sampler1D rainbow_tx;
uniform bool is_julia;
uniform int iterations;
varying vec4 p;
const vec4 BLACK = vec4( 0.0, 0.0, 0.0, 1.0 );

const vec3 color_map[] = {
	{ 0.0, 0.0, 0.1 },
	{ 0.1, 0.05, 0.15 },
	{ 0.15, 0.05, 0.2 },
	{ 0.2, 0.1, 0.3 },
	{ 0.2, 0.1, 0.5 },
	{ 0.25, 0.25, 0.65 },
	{ 0.325, 0.35, 0.7 },
	{ 0.4, 0.475, 0.6 },
	{ 0.6, 0.5, 0.4 },
	{ 0.725, 0.525, 0.35 }
};

void main( void )
{
	vec2 z = ( is_julia ) ? p.xy : vec2( 0.0, 0.0 );
	vec2 c = ( is_julia ) ? const_complex : p.xy;
	int i = 0;
	float n = 0.0;
	
	for ( ; i < iterations; i++ ) {
		vec2 _z = z;
		z = vec2( ( z.x * z.x ) - ( z.y * z.y ), 2 * ( z.x * z.y ) ) + c;
		if ( sqrt( length( z ) ) > 4.0 ) break;
		n = n + exp( -sqrt( length( z ) ) - 0.5 / ( sqrt( length( _z - z ) ) ) );
	}
		
	if ( i == iterations ) {
		gl_FragColor = BLACK;
	} else { 
		//gl_FragColor = texture1D( rainbow_tx, n ).rgba;
		gl_FragColor = vec4( color_map[( i * 100 / iterations % 10 )], 1.0 );
	}
}
