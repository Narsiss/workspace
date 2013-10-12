// TriangleShader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <GLTools.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager shaderManager;
GLBatch triangleBatch;
GLint shaderIdentity;

void Init()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	shaderManager.InitializeStockShaders();

	GLfloat vVrts[] = {0.0f,1.0f,0.0f,
								 -1.0,-1.0,0.0f,
								 1.0f,-1.0f,0.0f};
	GLfloat vColor[] = {1.0,0.0,0.0,1.0f,
									0.0f,1.0f,0.0f,1.0f,
									0.0f,0.0f,1.0f,1.0f};
	triangleBatch.Begin(GL_TRIANGLES,3);
		triangleBatch.CopyColorData4f(vColor);
		triangleBatch.CopyVertexData3f(vVrts);
	triangleBatch.End();

	shaderIdentity = gltLoadShaderPairWithAttributes("Triangle.vp","Triangle.fp",2,GLT_ATTRIBUTE_COLOR,"color",
																						GLT_ATTRIBUTE_VERTEX,"vertex");
}

void Reshape(int w,int h)
{
	glViewport(0,0,w,h);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(shaderIdentity);
	triangleBatch.Draw();

	glutSwapBuffers();
}

void Shutdown()
{
	glDeleteProgram(shaderIdentity);
}


int _tmain(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800,600);
	glutCreateWindow("test for shader");
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);

	GLenum err = glewInit();
	if(err!= GLEW_OK)
	{
		fprintf(stderr,"glew error: %s\n",glewGetErrorString(err));
		exit(1);
	}
	Init();
	glutMainLoop();

	Shutdown();
	return 0;
}

