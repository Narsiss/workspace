// UniformShader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrame viewFrame;
GLFrustum viewFrunstum;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;

GLuint flatShader;
GLint locMVP;
GLint locColor;

GLTriangleBatch torusBatch;

void Init()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	viewFrame.MoveForward(4.0);

	gltMakeTorus(torusBatch,0.8,0.3,52,26);

	flatShader = gltLoadShaderPairWithAttributes("FlatShader.vp","FlatShader.fp",1,GLT_ATTRIBUTE_VERTEX,"vVertex");

	locMVP = glGetUniformLocation(flatShader,"mvpMatrix");
	locColor = glGetUniformLocation(flatShader,"vColorValue");
}

void Reshape(int w,int h)
{
	if(h==0) h =1;
	glViewport(0,0,800,600);

	viewFrunstum.SetPerspective(35.0,(float)w/(float)h,1.0,100.0);
	projectionMatrix.LoadMatrix(viewFrunstum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static CStopWatch rotTimer;
	GLfloat rot = rotTimer.GetElapsedSeconds()*60.0;
	GLfloat vColor[] = {0.0,1.0,0.0,1.0f};
	modelViewMatrix.PushMatrix(viewFrame);
		modelViewMatrix.Rotate(rot,0.0,1.0f,0.0f);
		glUseProgram(flatShader);
		glUniform4fv(locColor,1,vColor);
		glUniformMatrix4fv(locMVP,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		torusBatch.Draw();
	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void ShutDown()
{
	glDeleteProgram(flatShader);
}


int _tmain(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_RGBA);
	glutInitWindowSize(800,600);
	glutCreateWindow("UniformShader");
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew error:%s",glewGetErrorString(err));
		exit(1);
	}
	Init();
	glutMainLoop();

	ShutDown();
	return 0;
}

