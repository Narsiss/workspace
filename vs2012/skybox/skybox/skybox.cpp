// skybox.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLGeometryTransform transformPipeline;
GLFrame viewFrame;
GLFrustum viewFrunstum;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;

GLBatch cubeBatch;
GLTriangleBatch sphereBatch;

GLuint texture;
GLint reflectionShader;
GLint skyBoxShader;

GLint locMVPReflect,locMVReflect,locNormalReflect,locInvertCamera;
GLint locMVPSkyBox;


void Init()
{
	const char *cubeFaces[6] = {"pos_x.tga","neg_x.tga","pos_y.tga","neg_y.tga","pos_z.tga","neg_z.tga"
												};
	GLenum cube[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
								GL_TEXTURE_CUBE_MAP_POSITIVE_Y,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
								GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
								};
	glClearColor(0.0,0.0,0.0,1.0);
	GLbyte *pBytes;
	int iWidth,iHeight,iComponent;
	GLenum eFormat;
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	//set texture imformation
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP,texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	//load texture
	for(int i = 0;i<6;i++)
	{
		pBytes = gltReadTGABits(cubeFaces[i],&iWidth,&iHeight,&iComponent,&eFormat);
		glTexImage2D(cube[i],0,iComponent,iWidth,iHeight,0,eFormat,GL_UNSIGNED_BYTE,pBytes);
		free(pBytes);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	//set view position
	viewFrame.MoveForward(-4.0);
		//init batch
	gltMakeSphere(sphereBatch, 1.0f, 52, 26);
    gltMakeCube(cubeBatch,20.0);
	//set two shaders
	reflectionShader = gltLoadShaderPairWithAttributes("reflection.vp","reflection.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",
																							GLT_ATTRIBUTE_NORMAL,"vNormal");
	locMVPReflect = glGetUniformLocation(reflectionShader,"mvpMatrix");
	locMVReflect = glGetUniformLocation(reflectionShader,"mvMatrix");
	locNormalReflect = glGetUniformLocation(reflectionShader,"normalMatrix");
	locInvertCamera = glGetUniformLocation(reflectionShader,"invertCamera");

	skyBoxShader = gltLoadShaderPairWithAttributes("skybox.vp","skybox.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",
																							GLT_ATTRIBUTE_NORMAL,"vNormal");
	locMVPSkyBox = glGetUniformLocation(skyBoxShader,"mvpMatrix");
}

void Display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	M3DMatrix44f mCamera,mInvertCamera,mCameraRot;
	m3dLoadIdentity44(mInvertCamera);
	viewFrame.GetCameraMatrix(mCamera,false);
	viewFrame.GetCameraMatrix(mCameraRot,true);
	//invert matrix
	m3dInvertMatrix44(mInvertCamera,mCameraRot);
	//draw reflection sphere
	/*
	modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(mCamera);
		glUseProgram(reflectionShader);
		glUniformMatrix4fv(locMVPReflect,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMVReflect,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
		glUniformMatrix3fv(locNormalReflect,1,GL_FALSE,transformPipeline.GetNormalMatrix());
		glUniformMatrix4fv(locInvertCamera,1,GL_FALSE,mInvertCamera);
		glEnable(GL_CULL_FACE);
		sphereBatch.Draw();
		glDisable(GL_CULL_FACE);
	modelViewMatrix.PopMatrix();
	*/
	//draw skybox
	
	modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(mCameraRot);
		glUseProgram(skyBoxShader);
		glUniformMatrix4fv(locMVPSkyBox,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
		cubeBatch.Draw();
	modelViewMatrix.PopMatrix();
	
	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
    {
    if(key == GLUT_KEY_UP)
        viewFrame.MoveForward(0.1f);

    if(key == GLUT_KEY_DOWN)
        viewFrame.MoveForward(-0.1f);

    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateLocalY(0.1);
      
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateLocalY(-0.1);
                        
    // Refresh the Window
    glutPostRedisplay();
}

void Reshape(int w,int h)
{
	if(h == 0) h =1;
	glViewport(0,0,w,h);

	viewFrunstum.SetPerspective(35.0,(float)w/(float)h,1.0,1000.0);
	projectionMatrix.LoadMatrix(viewFrunstum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}

void ShutdownRC()
{
	glDeleteTextures(1,&texture);
	glDeleteProgram(reflectionShader);
	glDeleteProgram(skyBoxShader);
}


int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(800,600);
	glutCreateWindow("my first sky box");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutSpecialFunc(SpecialKeys);

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew error: %s\n",glewGetErrorString(err));
		exit(1);
	}

	Init();
	glutMainLoop();

	ShutdownRC();
	return 0;
}

