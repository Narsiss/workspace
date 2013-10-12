

// uniformTest.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLTriangleBatch sphereBatch;
GLFrustum viewFrustum;
GLFrame viewFrame;
GLGeometryTransform transformPipeline;
GLMatrixStack modelviewMatrix;
GLMatrixStack projectionMatrix;
GLShaderManager shaderManager;
GLuint diffShader;
GLint locLightPosition;
GLint locMVP;
GLint locNormal;
GLint locMV;
GLint locDiffuseColor;
GLint locAmbientColor;
GLint locSpecularColor;

void Init()
{
	glClearColor(0.3,0.3,0.3,1.0);
	shaderManager.InitializeStockShaders();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	

	viewFrame.MoveForward(-20.0);
	//init spherebatch
	gltMakeTorus(sphereBatch,1.0,0.4,52,52);
	//init shader imformation
	diffShader = shaderManager.LoadShaderPairWithAttributes("phongShader.vp","phongShader.fp",2,GLT_ATTRIBUTE_VERTEX,
																				"vVertex",GLT_ATTRIBUTE_NORMAL,"vNormal");
	locDiffuseColor = glGetUniformLocation(diffShader,"diffuseColor");
	locAmbientColor = glGetUniformLocation(diffShader,"ambientColor");
	locSpecularColor = glGetUniformLocation(diffShader,"specularColor");
	locLightPosition = glGetUniformLocation(diffShader,"lightPosition");
	locMV = glGetUniformLocation(diffShader,"mvMatrix");
	locMVP = glGetUniformLocation(diffShader,"mvpMatrix");
	locNormal = glGetUniformLocation(diffShader,"normalMatrix");
}

void Reshadpe(int w,int h)
{
	if(h == 0) h =1;
	glViewport(0,0,w,h);

	viewFrustum.SetPerspective(35.0,(float)w/(float)h,1.0,100.0);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelviewMatrix,projectionMatrix);
}

CStopWatch rotTimer;

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat vDiffuseColor[] = {0.0,0.0,1.0,1.0};
	GLfloat vAmbientColor[] = {0.1,0.1,0.1,1.0};
	GLfloat vSpecularColor[] = {1.0,1.0,1.0,1.0};
	GLfloat vLightPosition[] = {5.0,5.0,0.0};
	//shadow matrix

	M3DMatrix44f mView;
	viewFrame.GetCameraMatrix(mView);
	modelviewMatrix.PushMatrix(mView);
		//modelviewMatrix.Translate(0.0,0.5,0.0);
		//modelviewMatrix.Rotate(rotTimer.GetElapsedSeconds()*60.0,0.0,1.0,0.0);
		glUseProgram(diffShader);
		glUniform4fv(locAmbientColor,1,vAmbientColor);
		glUniform4fv(locDiffuseColor,1,vDiffuseColor);
		glUniform4fv(locSpecularColor,1,vSpecularColor);
		glUniform3fv(locLightPosition,1,vLightPosition);
		glUniformMatrix4fv(locMV,1,GL_FALSE,modelviewMatrix.GetMatrix());
		glUniformMatrix3fv(locNormal,1,GL_FALSE,transformPipeline.GetNormalMatrix());
		glUniformMatrix4fv(locMVP,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		sphereBatch.Draw();
	modelviewMatrix.PopMatrix();
	//draw a shadow
	M3DMatrix44f mShadow;
	m3dLoadIdentity44(mShadow);
	modelviewMatrix.PushMatrix(mView);
		//modelviewMatrix.Translate(vLightPosition[0],vLightPosition[1],vLightPosition[2]);
		modelviewMatrix.MultMatrix(mShadow);
		//modelviewMatrix.Translate(-vLightPosition[0],-vLightPosition[1],-vLightPosition[2]);
		glUniformMatrix4fv(locMV,1,GL_FALSE,modelviewMatrix.GetMatrix());
		glUniformMatrix3fv(locNormal,1,GL_FALSE,transformPipeline.GetNormalMatrix());
		glUniformMatrix4fv(locMVP,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		sphereBatch.Draw();
	modelviewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void ShutdownRC()
{
	glDeleteShader(diffShader);
}


int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("test for diffuse light");
	glutReshapeFunc(Reshadpe);
	glutDisplayFunc(Display);

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew err:  %s\n",glewGetErrorString(err));
		exit(1);
	}

	Init();
	glutMainLoop();
	ShutdownRC();
	return 0;
}
