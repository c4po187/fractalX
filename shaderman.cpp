// shaderman.cpp

#include "shaderman.h"
#include <stdexcept>
#include <fstream>

shaderman::shaderman( const std::string vertexShaderFile, const std::string fragmentShaderFile )
{
	if ( !vertexShaderFile.empty() && !fragmentShaderFile.empty() ) {
		uVtx = glCreateShader( GL_VERTEX_SHADER );
		uFrag = glCreateShader( GL_FRAGMENT_SHADER );
		
		std::cout << "Shader Logs\n-----------" << std::endl;
		if ( compile( ST_VERTEX, vertexShaderFile ) < 0 )
			throw std::runtime_error( "Error: failed to compile vertex shader" );
		if ( compile( ST_FRAGMENT, fragmentShaderFile ) < 0 )
			throw std::runtime_error( "Error: failed to compile fragment shader" );
		
		GLint link;
		uProg = glCreateProgram();
		glAttachShader( uProg, uVtx );
		glAttachShader( uProg, uFrag );
		glLinkProgram( uProg );
		glGetProgramiv( uProg, GL_LINK_STATUS, &link );
		std::cout << "Program Log\n-----------" << std::endl;
		this->log( 0 );
		
		if ( link == GL_FALSE )
			throw std::runtime_error( "Error: failed to link shader program" );
	}
}

shaderman::~shaderman( void )
{
	glDetachShader( uProg, uVtx );
	glDetachShader( uProg, uFrag );
	glDeleteShader( uVtx );
	glDeleteShader( uFrag );
	glDeleteProgram( uProg );
}

int shaderman::compile( SHADERTYPE type, const std::string & filename )
{
	GLint compileStatus;
	std::cout << "Shader Type: " << type << std::endl;
	
	if ( !filename.empty() ) { 
		std::ifstream shaderFile( filename );
		
		if ( !shaderFile.is_open() ) {
			throw std::runtime_error( "Error: file does not exist" );
			return -2;
		}
		
		std::istreambuf_iterator< char > sf_begin( shaderFile );
		std::istreambuf_iterator< char > sf_end;
		std::string dataStr = std::string( sf_begin, sf_end );
		const char * data = dataStr.c_str();
		std::cout << "Shader filename: " << filename << std::endl;
		glShaderSource( ( type == ST_VERTEX ) ? uVtx : uFrag, 1, &data, NULL );
		glCompileShader( ( type == ST_VERTEX ) ? uVtx : uFrag );
		glGetShaderiv( ( type == ST_VERTEX ) ? uVtx : uFrag, GL_COMPILE_STATUS, &compileStatus );
		this->log( type );
		if ( compileStatus == GL_FALSE ) {
			throw std::runtime_error( "Error: failed to compile shader source" );
			return -3;
		}
	} else return -1;
	
	return 0;
}

void shaderman::log( SHADERTYPE type )
{
	int szlog;
	int len;
	if ( type > 0 ) {
		std::cout << "Running getShaderiv..." << std::endl;
		glGetShaderiv( ( type == ST_VERTEX ) ? uVtx : uFrag, GL_INFO_LOG_LENGTH, &szlog );
	} else {
		std::cout << "Running getProgramiv..." << std::endl;
		glGetProgramiv( uProg, GL_INFO_LOG_LENGTH, &szlog );
	}
	if ( szlog > 1 ) {
		std::cout << "Log size: " << szlog << std::endl;
		char buffer[1024];
		if ( type > 0 ) {
			glGetShaderInfoLog( ( type == ST_VERTEX ) ? uVtx : uFrag, 1024, &len, buffer );
			std::cout << "SHADER LOG: " << buffer << std::endl;
		} else {
			glGetProgramInfoLog( uProg, 1024, &len, buffer );
			std::cout << "PROGRAM LOG: " << buffer << std::endl;
		}
	} else {
		std::cout << "No errors to report..." << std::endl;
	}
	std::cout << std::endl;
}

// END-SRC
