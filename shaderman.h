#ifndef __SHADERMAN_H__
#define __SHADERMAN_H__ 1

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <string>

enum
{
	ST_VERTEX = 1,
	ST_FRAGMENT = 2
};

typedef unsigned short SHADERTYPE;

class shaderman
{
	public:
		explicit shaderman( const std::string, const std::string );
		~shaderman( void );
		
		inline const GLuint & getProgram( void ) const { return uProg; }
		
	private:
		int compile( SHADERTYPE, const std::string & );
		void log( SHADERTYPE );
	
		GLuint uVtx, uFrag, uProg;
};

#endif /* __SHADERMAN_H__ */
