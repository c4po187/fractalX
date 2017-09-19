// Julia (main.cpp)

#include "shaderman.h"
#include "argparse.hpp"
#include <cmath>
#include <cstdlib>

#define WIN_WIDTH 1280
#define WIN_HEIGHT 960
#define ZMOD 0.01f
#define XYMOD 0.0085f

// Globals
static shaderman * shm;
static GLuint buffer;
static const float vertices[] = {
	-1.0, 1.0, 0.0,
	1.0, 1.0, 0.0,
	-1.0, -1.0, 0.0,
	1.0, -1.0, 0.0
};
static float color0[] = { 1.0, 1.0, 1.0, 1.0 };
static float color1[] = { 0.15, 0.3, 0.45, 1.0 };
static float color2[] = { 0.45, 0.6, 0.75, 1.0 };
static float color3[] = { 0.75, 0.9, 1.0, 1.0 };
static float color4[] = { 1.0, 0.85, 0.7, 1.0 };
static float color5[] = { 0.7, 0.55, 0.4, 1.0 };
static float color6[] = { 0.4, 0.25, 0.1, 1.0 };
static float center[] = { -0.5, 0.0, 0.0, 0.0 };
static float constComplex[] = { -0.0825, -0.65 };
static float tx, ty, tz, divit = 1.0f;
static int dx, dy, iterations;
static bool d = false, doZoom = false, doMove = false, isJulia = true;

static void init( void )
{
	tx = ty = 0.0f;
	tz = 1.0f;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
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
			doMove = true;
			break;
		case 's': 
			ty -= XYMOD * divit; 
			doMove = true;
			break;
		case 'a': 
			tx -= XYMOD * divit; 
			doMove = true;
			break;
		case 'd': 
			tx += XYMOD * divit; 
			doMove = true;
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
			doZoom = true;
			break;
		case GLUT_KEY_PAGE_DOWN:
			divit += 0.0015f;
			tz -= ZMOD;
			doZoom = true;
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
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color0" ), 1, color0 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color1" ), 1, color1 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color2" ), 1, color2 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color3" ), 1, color3 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color4" ), 1, color4 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color5" ), 1, color5 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "color6" ), 1, color6 );
		glUniform4fv( glGetUniformLocation( shm->getProgram(), "center" ), 1, center );
		glUniform2fv( glGetUniformLocation( shm->getProgram(), "const_complex" ), 1, constComplex );
		glUniform1f( glGetUniformLocation( shm->getProgram(), "aspect" ), 
			static_cast< float >( WIN_WIDTH / WIN_HEIGHT ) );
		glUniform1i( glGetUniformLocation( shm->getProgram(), "is_julia" ), isJulia );
		glUniform1i( glGetUniformLocation( shm->getProgram(), "iterations" ), iterations );
		
		if ( doZoom ) {
			glScalef( tz, tz, tz );
			doZoom = false;
			tz = 1.0f;
		}
			
		glEnableClientState( GL_VERTEX_ARRAY );
		glBindBuffer( GL_ARRAY_BUFFER, buffer );
		glVertexPointer( 3, GL_FLOAT, 0, 0 );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
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
		std::cout << "\nNo parameters supplied, using defaults...\nSET: Mandelbrot\nITERATIONS: 256\n" << std::endl;
		isJulia = false;
		iterations = 256;
	} else {
		if ( set.empty() ) {
			std::cout << parser.usage() << std::endl;
			return 1;
		} else if ( set == "j" || set == "J" || set == "julia" || set == "Julia" ) {
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
		} else if ( set == "m" || set == "M" || set == "mandelbrot" || set == "Mandelbrot" ) {
			isJulia = false;
		} else {
			std::cout << parser.usage() << std::endl;
			return 3;
		}
		try {
			iterations = atoi( s_iterations.c_str() );
		} catch ( std::exception & e ) {
			std::cout << e.what() << std::endl;
		}
	}
		
	glutInit( &argc, const_cast< char ** >( argv ) );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
	glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );
	glutCreateWindow( "Julia Set Renderer" );
	
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
	return 0;
}

// END-SRC