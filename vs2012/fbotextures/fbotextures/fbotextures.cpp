// fbotextures.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include <iostream>

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLBatch.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#include <GL/GLU.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

static GLfloat vGreen[] = {0.0f,1.0f,0.0f,1.0f};
static GLfloat vWhite[] = {1.0f,1.0f,1.0f,1.0f};
static GLfloat vBlue[] = {0.0f,0.0f,1.0f,1.0f};
static GLfloat vGrey[] = {0.5f,0.5f,0.5f,1.0f};
static GLfloat vBlack[] = {0.0f,0.0f,0.0f,1.0f};

static GLfloat vLightPosition[] = {-2.0f,3.0f,-2.0f,1.0f};

static GLenum windowBuffer[] = {GL_BACK_LEFT};
static GLenum fboBuffers[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
static GLint mirrorWidth = 800;
static GLint mirrorHeight = 800;

GLsizei screenWidth;
GLsizei screenHeight;

GLboolean isFullScreen;
GLboolean isAnimated;

GLboolean isMoveForward = false;
GLboolean isMoveBack = false;
GLboolean isRotateLeft = false;
GLboolean isRotateRight = false;
GLboolean isRotateUp = false;
GLboolean isRotateDown = false;

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;
GLFrame cameraFrame;
GLFrame mirrorFrame;

GLTriangleBatch torusBatch;
GLTriangleBatch sphereBatch;
GLTriangleBatch cylinderBatch;

GLBatch floorBatch;
GLBatch mirrorFrontBatch;
GLBatch mirrorBackBatch;

GLuint fboName;
GLuint textures[1];
GLuint mirrorTexture;
GLuint depthBufferName;

bool LoadBMPTexture(const char *bmpFileName,GLenum minFilter,GLenum magFilter,GLenum wrapMode)
{
	GLbyte* pBits;
	GLint width,height;

	pBits = gltReadBMPBits(bmpFileName,&width,&height);

	//texure wrap mode
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrapMode);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrapMode);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,pBits);


	if(minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_NEAREST)
		glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}


void initTriangleBatch()
{
	 gltMakeTorus(torusBatch,1.0f,0.5f,50,25);
	 gltMakeSphere(sphereBatch,0.3,32,32);
	 gltMakeCylinder(cylinderBatch,1.0,0.5,1.0,16,32);
}

void initMirrorBatch(GLBatch* batch)
{
	batch->Begin(GL_TRIANGLE_FAN,4,1);
		batch->Color4f(1.0f,0.0f,0.0f,1.0f);
		batch->Normal3f(0.0f,1.0f,0.0f);
		batch->MultiTexCoord2f(0,0.0f,0.0f);
		batch->Vertex3f(-5.0f,0.0f,-20.0f);

		batch->Color4f(1.0f,0.0f,0.0f,1.0f);
		batch->Normal3f(0.0f,1.0f,0.0f);
		batch->MultiTexCoord2f(0,1.0f,0.0f);
		batch->Vertex3f(5.0f,0.0f,-20.0f);

		batch->Color4f(1.0f,0.0f,0.0f,1.0f);
		batch->Normal3f(0.0f,1.0f,0.0f);
		batch->MultiTexCoord2f(0,1.0f,1.0f);
		batch->Vertex3f(5.0f,5.0f,-20.0f);

		batch->Color4f(1.0f,0.0f,0.0f,1.0f);
		batch->Normal3f(0.0f,1.0f,0.0f);
		batch->MultiTexCoord2f(0,0.0f,1.0f);
		batch->Vertex3f(-5.0f,5.0f,-20.0f);
	batch->End();
}

void initFloorBatch()
{	
	float alpha = 0.25f;

	floorBatch.Begin(GL_TRIANGLE_FAN,4,1);
		floorBatch.Color4f(0.0f,1.0f,0.0f,alpha);
		floorBatch.MultiTexCoord2f(0,0.0,0.0);
		floorBatch.Normal3f(0.0,1.0,0.0);
		floorBatch.Vertex3f(-20.0f,-0.4f,20.0f);

		floorBatch.Color4f(0.0f,1.0f,0.0f,alpha);
		floorBatch.MultiTexCoord2f(0,10.0,0.0);
		floorBatch.Normal3f(0.0,1.0,0.0);
		floorBatch.Vertex3f(20.0f,-0.4f,20.0f);

		floorBatch.Color4f(0.0f,1.0f,0.0f,alpha);
		floorBatch.MultiTexCoord2f(0,10.0,10.0);
		floorBatch.Normal3f(0.0,1.0,0.0);
		floorBatch.Vertex3f(20.0f,-0.4f,-20.0f);

		floorBatch.Color4f(0.0f,1.0f,0.0f,alpha);
		floorBatch.MultiTexCoord2f(0,0.0,10.0);
		floorBatch.Normal3f(0.0,1.0,0.0);
		floorBatch.Vertex3f(-20.0f,-0.4f,-20.0f);
	floorBatch.End();
}


void Init()
{
	//init glew
	GLenum err = glewInit();
	if(err != GLEW_OK) fprintf(stderr,"error : %s\n",glewGetErrorString(err));
		
	//init clear color
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	
	//init shader manager
	shaderManager.InitializeStockShaders();
	
	//init enable functions
	glEnable(GL_DEPTH_TEST);

	//init batches
	initTriangleBatch();
	initFloorBatch();
	initMirrorBatch(&mirrorFrontBatch);

	//bind floor texture
	glGenTextures(1,textures);
	glBindTexture(GL_TEXTURE_2D,textures[0]);
	LoadBMPTexture("Marble.bmp",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_REPEAT);

	//create and bind a FBO
	glGenFramebuffers(1,&fboName);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fboName);

	//create depth renderbuffer
	glGenRenderbuffers(1,&depthBufferName);
	glBindRenderbuffer(GL_RENDERBUFFER,depthBufferName);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32,mirrorWidth,mirrorHeight);

	//create reflection texture for mirror
	glGenTextures(1,&mirrorTexture);
	glBindTexture(GL_TEXTURE_2D,mirrorTexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,mirrorWidth,mirrorHeight,0,GL_RGBA,GL_FLOAT,NULL);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,mirrorTexture,0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthBufferName);

	gltCheckErrors();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void Reshape(int width,int height)
{
	glViewport(0,0,width,height);
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);

	viewFrustum.SetPerspective(35.0f,float(width)/float(height),1.0f,100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	modelViewMatrix.LoadIdentity();

	screenHeight = height;
	screenWidth = width;
}


void ShutDown()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER,0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);

	glDeleteTextures(1,&mirrorTexture);
	glDeleteTextures(1,textures);

	glDeleteRenderbuffers(1,&depthBufferName);
	glDeleteFramebuffers(1,&fboName);
}

void drawSun()
{
	//draw the sphere batch as a sun
	modelViewMatrix.PushMatrix();
		modelViewMatrix.Translatev(vLightPosition);
		shaderManager.UseStockShader(GLT_SHADER_FLAT,
			transformPipeline.GetModelViewProjectionMatrix(),vWhite);
		sphereBatch.Draw();
	modelViewMatrix.PopMatrix();
}

void drawTorus(GLfloat yRot)
{
	M3DMatrix44f mCamera;
	M3DVector4f vLightTransform;
	modelViewMatrix.GetMatrix(mCamera);
	m3dTransformVector4(vLightTransform,vLightPosition,mCamera);

	modelViewMatrix.PushMatrix();
		modelViewMatrix.Translate(0.0,0.5,-1.0);
		modelViewMatrix.Rotate(yRot,0.0,1.0,0.0);
		shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
			modelViewMatrix.GetMatrix(),
			transformPipeline.GetProjectionMatrix(),
			vLightTransform,vGreen,0);
		torusBatch.Draw();
	modelViewMatrix.PopMatrix();
}



void Display()
{
	static CStopWatch timer;
	GLfloat yRot = timer.GetElapsedSeconds() * 60.0;

	M3DVector3f vCameraPosition;
	M3DVector3f vCameraForward;
	M3DVector3f vMirrorPosition;
	M3DVector3f vMirrorForward;
	void movingCylinder();

	cameraFrame.GetOrigin(vCameraPosition);
	cameraFrame.GetForwardVector(vCameraForward);

	vMirrorPosition[0] = 0.0f;
	vMirrorPosition[1] = 0.1f;
	vMirrorPosition[2] = -20.0f;
	mirrorFrame.SetOrigin(vMirrorPosition);

	vMirrorForward[0] = vCameraPosition[0];
	vMirrorForward[1] = vCameraPosition[1];
	vMirrorForward[2] = (vCameraPosition[2] + 20);
	m3dNormalizeVector3(vMirrorForward);
	mirrorFrame.SetForwardVector(vMirrorForward);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fboName);
	glDrawBuffers(1,fboBuffers);
	glViewport(0,0,mirrorWidth,mirrorHeight);

	modelViewMatrix.PushMatrix();
		M3DMatrix44f mMirrorView;
		mirrorFrame.GetCameraMatrix(mMirrorView);
		modelViewMatrix.MultMatrix(mMirrorView);
		modelViewMatrix.Scale(-1.0f,1.0f,1.0f);
			glBindTexture(GL_TEXTURE_2D,textures[0]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,
				transformPipeline.GetModelViewProjectionMatrix(),
				vWhite,0);
			floorBatch.Draw();

			drawSun();
			drawTorus(yRot);
	modelViewMatrix.PopMatrix();
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glDrawBuffers(1,windowBuffer);
	glViewport(0,0,mirrorWidth,mirrorHeight);
	
	modelViewMatrix.PushMatrix();
		M3DMatrix44f mCamera;
		cameraFrame.GetCameraMatrix(mCamera);
		modelViewMatrix.MultMatrix(mCamera);

		modelViewMatrix.PushMatrix();
			glBindTexture(GL_TEXTURE_2D,mirrorTexture);
			shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,
				transformPipeline.GetModelViewProjectionMatrix(),0);
			mirrorFrontBatch.Draw();
		modelViewMatrix.PopMatrix();
		modelViewMatrix.PushMatrix();
			glBindTexture(GL_TEXTURE_2D,textures[0]);
			shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,
				transformPipeline.GetModelViewProjectionMatrix(),
				vWhite,0);
			floorBatch.Draw();

			drawSun();
			drawTorus(yRot);
		modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();


	//control to moving cylinder
	movingCylinder();

	glutSwapBuffers();
	glutPostRedisplay();
}

void movingCylinder()
{

	static CStopWatch cameraTimer;
	float fTime = cameraTimer.GetElapsedSeconds();
	cameraTimer.Reset(); 

	float linear = fTime * 3.0f;
	float angular = fTime * float(m3dDegToRad(15.0f));

	if(isMoveForward == true) cameraFrame.MoveForward(linear);
	if(isMoveBack == true) cameraFrame.MoveForward(-linear);
	if(isRotateLeft == true) cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
	if(isRotateRight == true) cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
	if(isRotateUp == true) cameraFrame.RotateWorld(angular, 1.0f, 0.0f, 0.0f);
	if(isRotateDown == true) cameraFrame.RotateWorld(-angular, 1.0f, 0.0f, 0.0f);
}

void SpecialKeys(int key,int x,int y)
{
	if(key == GLUT_KEY_UP)
		isMoveForward = true;
	if(key == GLUT_KEY_DOWN)
		isMoveBack = true;
	if(key == GLUT_KEY_LEFT)
		isRotateLeft = true;
	if(key == GLUT_KEY_RIGHT)
		isRotateRight = true;
	if(key == GLUT_KEY_PAGE_UP)
		isRotateUp = true;
	if(key == GLUT_KEY_PAGE_DOWN)
		isRotateDown = true;
}

void SpecialKeysUp(int key,int x,int y)
{
	if(key == GLUT_KEY_UP)
		isMoveForward = false;
	if(key == GLUT_KEY_DOWN)
		isMoveBack = false;
	if(key == GLUT_KEY_LEFT)
		isRotateLeft = false;
	if(key == GLUT_KEY_RIGHT)
		isRotateRight = false;
	if(key == GLUT_KEY_PAGE_UP)
		isRotateUp = false;
	if(key == GLUT_KEY_PAGE_DOWN)
		isRotateDown = false;
}

int _tmain(int argc, char* argv[])
{
	screenHeight = 600;
	screenWidth = 800;
	isFullScreen = false;
	isAnimated = true;
	
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth,screenHeight);
	
	glutCreateWindow("FBO Mirror");

	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutSpecialFunc(SpecialKeys);
	glutSpecialUpFunc(SpecialKeysUp);

	Init();
	glutMainLoop();
	ShutDown();

	return 0;
}

