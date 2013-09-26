// shadowTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <math3d.h>

#include <myMath3d.h>

#define FREEGLUT_STATIC
#include <GL/glut.h>


GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrame viewFrame;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager;

GLBatch triangleBatch;
GLuint shader;
GLint locMVPMatrix;
GLint locMVMatrix;
GLint locNormalMatrix;
GLint locLightPosition;
GLint locDiffuseColor;
GLint locAmbientColor;
GLint locSpecularColor;

GLTriangleBatch sphereBatch;

void Init()
{
	glClearColor(0.3,0.3,0.3,1.0);
	glEnable(GL_DEPTH_TEST);
	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(-30.0);

	shader = shaderManager.LoadShaderPairWithAttributes("phongShader.vp","phongShader.fp",2,GLT_ATTRIBUTE_VERTEX,
																				"vVertex",GLT_ATTRIBUTE_NORMAL,"vNormal");
	locDiffuseColor = glGetUniformLocation(shader,"diffuseColor");
	locAmbientColor = glGetUniformLocation(shader,"ambientColor");
	locSpecularColor = glGetUniformLocation(shader,"specularColor");
	locLightPosition = glGetUniformLocation(shader,"lightPosition");
	locMVMatrix = glGetUniformLocation(shader,"mvMatrix");
	locMVPMatrix = glGetUniformLocation(shader,"mvpMatrix");
	locNormalMatrix = glGetUniformLocation(shader,"normalMatrix");

	triangleBatch.Begin(GL_TRIANGLES,3);
		triangleBatch.Normal3f(0.0,1.0,0.0);
		triangleBatch.Vertex3f(-1.0,0.3,0.0);
		triangleBatch.Normal3f(0.0,1.0,0.0);
		triangleBatch.Vertex3f(1.0,0.3,0.0);
		triangleBatch.Normal3f(0.0,1.0,0.0);
		triangleBatch.Vertex3f(0.0,1.0,-1.0);
	triangleBatch.End();
}

void Reshape(int w,int h)
{
	if(h == 0) h = 1;
	glViewport(0,0,w,h);

	viewFrustum.SetPerspective(35.0,(float)w/(float)h,1.0,100.0);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}


void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat vLightPosition[] = {5.0,5.0,5.0};
	GLfloat vAmbientColor[] = {0.1,0.1,0.1,1.0};
	GLfloat vDiffuseColor[] = {1.0,0.0,0.0,1.0};
	GLfloat vSpecularColor[] = {1.0,1.0,1.0,1.0};
	modelViewMatrix.PushMatrix();
		M3DMatrix44f mCamera;
		viewFrame.GetCameraMatrix(mCamera);
		//modelViewMatrix.MultMatrix(mCamera);
		modelViewMatrix.Translate(0.0,0,-10.0);
		glUseProgram(shader);
		glUniform4fv(locAmbientColor,1,vAmbientColor);
		glUniform4fv(locDiffuseColor,1,vDiffuseColor);
		glUniform4fv(locSpecularColor,1,vSpecularColor);
		glUniform3fv(locLightPosition,1,vLightPosition);
		glUniformMatrix4fv(locMVPMatrix,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMVMatrix,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
		glUniformMatrix3fv(locNormalMatrix,1,GL_FALSE,transformPipeline.GetNormalMatrix());
		triangleBatch.Draw();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PushMatrix();
		viewFrame.GetCameraMatrix(mCamera);
		M3DMatrix44f mTest;
		GLfloat flat[] = {0.0,1.0,1.0,3.0};
		m3dMakeShadowMatrix(mTest,flat,vLightPosition);
		modelViewMatrix.Translate(0.0,0,-10.0);
		modelViewMatrix.MultMatrix(mTest);
		glUseProgram(shader);
		glUniformMatrix4fv(locMVPMatrix,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMVMatrix,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
		glUniformMatrix3fv(locNormalMatrix,1,GL_FALSE,transformPipeline.GetNormalMatrix());
		triangleBatch.Draw();
	modelViewMatrix.PopMatrix();
	glutSwapBuffers();

	//glutPostRedisplay();
		
}

int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("test");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
		exit(1);
	}
	Init();
	glutMainLoop();


	return 0;
}

