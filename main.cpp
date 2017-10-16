// Julia (main.cpp)

#include "shaderman.h"
#include "vertex.h"
#include "argparse.hpp"
#include <cmath>
#include <cstdlib>

#define WIN_WIDTH 1280
#define WIN_HEIGHT 960
#define ZMOD 0.01f
#define XYMOD 0.0085f
#define SZTX 1024

// Globals
static shaderman * shm;
static GLuint vao, vbo, vtx;
static vertex * verts;
static GLfloat rtex[SZTX][4];
static const float color0[] = { 1.0, 1.0, 1.0, 1.0 };
static const float color1[] = { 0.15, 0.3, 0.45, 1.0 };
static const float color2[] = { 0.45, 0.6, 0.75, 1.0 };
static const float color3[] = { 0.75, 0.9, 1.0, 1.0 };
static const float color4[] = { 1.0, 0.85, 0.7, 1.0 };
static const float color5[] = { 0.7, 0.55, 0.4, 1.0 };
static const float color6[] = { 0.4, 0.25, 0.1, 1.0 };
static float center[] = { -0.5, 0.0, 0.0, 0.0 };
static float constComplex[] = { 0.1875, 0.565 };
static float tx, ty, tz, divit = 1.0f;
static int dx, dy, iterations = 256;
static bool d = false, isJulia = true;

static void setupVertexPositions( void )
{
	if ( verts ) {
		verts[0].pos = vec4( -1.0, 1.0, 0.0, 1.0 );
		verts[1].pos = vec4( 1.0, 1.0, 0.0, 1.0 );
		verts[2].pos = vec4( -1.0, -1.0, 0.0, 1.0 );
		verts[3].pos = vec4( 1.0, -1.0, 0.0, 1.0 );
	}
}

static void setupVertexTexCoords( void )
{
	if ( verts ) {
		verts[0].uv = vec2( center[0] - tz, center[1] + tz );
		verts[1].uv = vec2( center[0] + tz, center[1] + tz );
		verts[2].uv = vec2( center[0] - tz, center[1] - tz );
		verts[3].uv = vec2( center[0] + tz, center[1] - tz );
	}
}

// Creates a rainbow texture
static void createTexture( void )
{	
	for ( int x = 0; x < SZTX - 1; ++x ) {
		float f = 3.1415927 * x / SZTX;
		float c = cos( f );
		float s = sin( f );
		rtex[x][0] = 1.0 - c;
		rtex[x][1] = s * s;
		rtex[x][2] = c;
		rtex[x][3] = 1.0;
	}
	
	rtex[SZTX - 1][0] = rtex[SZTX - 1][1] = rtex[SZTX - 1][2] = rtex[SZTX - 1][3] = 0.0;
	glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, SZTX, 0, GL_RGBA, GL_FLOAT, rtex );
}

static void init( void )
{
	tx = ty = 0.0f;
	tz = 1.0f;
	verts = static_cast< vertex * >( malloc( 4 * sizeof( vertex ) ) );
	setupVertexPositions();
	setupVertexTexCoords();
	
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glGenTextures( 1, &vtx );
	glBindTexture( GL_TEXTURE_1D, vtx );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glEnable( GL_TEXTURE_1D );
	createTexture();
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, 4 * sizeof( vertex ), verts, GL_STATIC_DRAW );
	
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( vertex ), 
		reinterpret_cast< void * >( offsetof( vertex, pos ) ) );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( vertex ),
		reinterpret_cast< void * >( offsetof( vertex, uv ) ) );
	glEnableVertexAttribArray( 1 );
	glBindVertexArray( 0 );
	
	// One timers
	if ( shm ) {
		glUseProgram( shm->getProgram() );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color0" ), 1, color0 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color1" ), 1, color1 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color2" ), 1, color2 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color3" ), 1, color3 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color4" ), 1, color4 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color5" ), 1, color5 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color6" ), 1, color6 );
		glUniform2fv( glGetUniformLocation( shm->getProgram(), "const_complex" ), 1, constComplex );
		glUniform1i( glGetUniformLocation( shm->getProgram(), "is_julia" ), isJulia );
		glUniform1i( glGetUniformLocation( shm->getProgram(), "iterations" ), iterations );
	}
	
	glViewport( 0, 0, WIN_WIDTH, WIN_HEIGHT );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	glOrtho( 0, 0, 
		static_cast< GLdouble >( WIN_WIDTH ), 
		static_cast< GLdouble >( WIN_HEIGHT ), 
		-1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
}

/*
static void viewfix( int x, int y, float * fx, float * fy )
{
	*fx = ( exp( zoom * log( 1.1 ) ) * 2.0 ) / ( WIN_WIDTH * x + center[0] - exp( zoom * log( 1.1 ) ) );
	*fy = ( exp( zoom * log( 1.1 ) ) * 2.0 ) / ( WIN_HEIGHT * y + center[1] - exp( zoom * log( 1.1 ) ) );
}
*/

/*** Callbacks **************************************/

void cb_keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
		case 'w': 
			ty += XYMOD * divit;
			break;
		case 's': 
			ty -= XYMOD * divit; 
			break;
		case 'a': 
			tx -= XYMOD * divit; 
			break;
		case 'd': 
			tx += XYMOD * divit;
			break;
		default: break;
	}
	center[0] += tx;
	center[1] += ty;
	tx = ty = 0;
}

void cb_special( int key, int x, int y )
{
	switch ( key ) {
		case GLUT_KEY_PAGE_UP:
			divit -= 0.0015f;
			tz += ZMOD;
			break;
		case GLUT_KEY_PAGE_DOWN:
			divit += 0.0015f;
			tz -= ZMOD;
			break;
		default: break;
	}
	if ( divit < 0.0001f ) divit = 0.0001f;
	if ( divit > 1.0f ) divit = 1.0f; 
}

void cb_mouse( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON ) {
		if ( state == GLUT_DOWN ) {
			std::cout << "Mouse Down!" << std::endl;
			d = true;
			dx = x;
			dy = y;
		} else if ( state == GLUT_UP ) {
			std::cout << "Mouse Up!" << std::endl;
			d = false;
		}
	}
}

// @TODO: Fix mouse motion
void cb_motion( int x, int y )
{
	if ( d ) {
		std::cout << "X: " << x << ", Y: " << y << std::endl;
		float fx = ( ( x - dx ) / ( WIN_WIDTH / 2 ) ) * exp( tz * log( 1.1 ) );
		float fy = ( ( y - dy ) / ( WIN_HEIGHT / 2 ) ) * exp( tz * log( 1.1 ) );
		std::cout << "fx: " << fx << ", fy: " << fy << std::endl;
		dx = x;
		dy = y;
		std::cout << "_c0: " << center[0] << ", _c1: " << center[1] << std::endl;
		center[0] += -fx;
		center[1] += fy;
		std::cout << "c0: " << center[0] << ", c1: " << center[1] << std::endl;
	}
}

void cb_displayRate( int val )
{
	glutPostRedisplay();
	glutTimerFunc( 20, cb_displayRate, 0 );
}

void cb_render( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	if ( shm ) {
		glUseProgram( shm->getProgram() );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "center" ), 1, center );
		glUniform1f( glGetUniformLocation( shm->getProgram(), "aspect" ), 
			static_cast< float >( WIN_WIDTH / WIN_HEIGHT ) );
		
		if ( tz != 1.0f ) {
			glScalef( tz, tz, tz );
			tz = 1.0f;
		}
		
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_1D, vtx );
		glEnableClientState( GL_VERTEX_ARRAY );
		glBindVertexArray( vao );
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glVertexPointer( 4, GL_FLOAT, 0, 0 );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glBindVertexArray( 0 );
		glDisableClientState( GL_VERTEX_ARRAY );
	} else {
		std::cout << "Shaderman failure!" << std::endl;
	}
	
	glutSwapBuffers();
}

/****************************************************/

int main( int argc, const char * argv[] )
{
	ArgumentParser parser;
	parser.addArgument( "-s", "--set", 1 );
	parser.addArgument( "-c", "--complex", 3 );
	parser.addArgument( "-i", "--iterations", 1 );
	parser.parse( argc, argv );
	std::string set = parser.retrieve< std::string >( "set" );
	std::string s_iterations = parser.retrieve< std::string >( "iterations" );
	std::vector< std::string > c_args = parser.retrieve< std::vector< std::string > >( "complex" );
	
	if ( set.empty() && c_args.empty() && s_iterations.empty() ) {
		std::cout << "No parameters supplied, using defaults...\nSET: Mandelbrot\nITERATIONS: 256\n" << std::endl;
		isJulia = false;
	} else {
		if ( set.empty() ) {
			std::cout << parser.usage() << std::endl;
			return 1;
		} else if ( set == "j" || set == "J" || set == "julia" || set == "Julia" ) {
			if ( c_args.size() ) {
				if ( c_args.size() == 3 && ( c_args.at( 1 ) == "+" || c_args.at( 1 ) == "-" ) ) {
					try {
						constComplex[0] = strtof( c_args.at( 0 ).c_str(), 0 );
						constComplex[1] = strtof( c_args.at( 2 ).c_str(), 0 );
						if ( c_args.at( 1 ) == "-" )
							constComplex[1] *= -1.0;
					} catch ( std::exception & e ) {
						std::cout << e.what() << std::endl;
					}
				} else {
					std::cout << parser.usage() << std::endl;
					std::cout << "Please enter complex number in correct format: a + bi" << std::endl;
					return 2;
				}
			} else {
				std::cout << "No complex number provided using 0.1875 + 0.565i" << std::endl;
			}
		} else if ( set == "m" || set == "M" || set == "mandelbrot" || set == "Mandelbrot" ) {
			isJulia = false;
		} else {
			std::cout << parser.usage() << std::endl;
			return 3;
		}
		if ( !s_iterations.empty() ) {
			try {
				iterations = atoi( s_iterations.c_str() );
			} catch ( std::exception & e ) {
				std::cout << e.what() << std::endl;
			}
		}
		// Display set params
		std::cout << "Running FractalX with specified parameters..." << std::endl;
		std::cout << "SET: ";
		if ( isJulia )
			std::cout << "Julia" << std::endl;
		else
			std::cout << "Mandelbrot" << std::endl;
		std::cout << "ITERATIONS: " << iterations << std::endl;
		if ( isJulia ) {
			std::cout << "COMPLEX: " << constComplex[0];
			char s = ( c_args.empty() ) ? '+' : c_args.at( 1 )[0];
			std::cout << ' ' << s << ' ' << fabs( constComplex[1] ) << "i" << std::endl;
		}
		std::cout << std::endl;
	}
		
	glutInit( &argc, const_cast< char ** >( argv ) );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
	glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );
	glutCreateWindow( "FractalX : OpenGL Fractal Explorer" );
	
	if ( glewInit() != GLEW_OK ) {
		std::cout << "Glew failed to init, exiting..." << std::endl;
		return 4;
	}
	
	try {
		shm = new shaderman( "./shaders/julia.vert", "./shaders/julia.frag" );
	} catch ( std::exception & e ) {
		std::cout << "ShaderManager: " << e.what() << std::endl;
		delete shm;
	}
	
	if ( !shm ) return 5;
	
	init();
	glutDisplayFunc( cb_render );
	glutTimerFunc( 20, cb_displayRate, 0 );
	glutKeyboardFunc( cb_keyboard );
	glutSpecialFunc( cb_special );
	glutMouseFunc( cb_mouse );
	glutMotionFunc( cb_motion );
	glutMainLoop();
	
	delete shm;
	if ( verts ) free( verts );
	
	return 0;
}

// END-SRC
