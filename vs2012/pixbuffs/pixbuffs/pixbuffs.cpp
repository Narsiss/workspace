// pixbuffs.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <GLShaderManager.h>
#include <GLTools.h>
#include <GLTriangleBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>
#include <StopWatch.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

static GLfloat vGreen[] = {0.0f,1.0f,0.0f,1.0f};
static GLfloat vWhite[] = {1.0f,1.0f,1.0f,1.0f};
static GLfloat vLightPosition[] = {0.0f,3.0f,0.0f,1.0f};
static const GLenum windowBuffer[] = {GL_BACK_LEFT};
static const GLenum fboBuffers[] = {GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};

GLsizei screenHeight;
GLsizei screenWidth;

GLboolean isFullScreen;
GLboolean isAnimated;

GLMatrixStack modelviewMatrix;
GLMatrixStack projectionMatrix;
M3DMatrix44f orthoMatrix;

GLShaderManager shaderManager;
GLFrustum viewFrustum;
GLFrame cameraFrame;
GLGeometryTransform transformPipeline;

GLTriangleBatch torusBatch;
GLBatch floorBatch;
GLBatch screenBatch;
GLBatch mapBatch;

GLuint textures[1];
GLuint blurTextures[6];
GLuint pixBufferObj[1];
GLuint curBlurTarget;

bool isUsePBO;
GLfloat speedFactor;
GLuint	blurShaderProg;
void *pixelData;
GLuint pixelDataSize;
GLuint fboName;
GLuint fboTexture;
GLuint depthFBOBuffer;

GLboolean isMoveForward;
GLboolean isMoveBack;
GLboolean isRotateLeft;
GLboolean isRotateRight;
GLboolean isRotateUp;
GLboolean isRotateDown;

void AdvanceBlurTarget()	{curBlurTarget = (curBlurTarget+1) % 6;}
int GetBlurTarget(int targetCount) {return ((curBlurTarget + 5 - targetCount)%6+1);}

bool LoadBMPTexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)	
{
	GLbyte *pBits;
	GLint iWidth, iHeight;

	pBits = gltReadBMPBits(szFileName, &iWidth, &iHeight);
	if(pBits == NULL)
		return false;

	// Set Wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	// Do I need to generate mipmaps?
	if(minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_NEAREST)
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pBits);
	return true;
}

void updateFrameCount()
{
	static int iFrames = 0;
	static CStopWatch timer;

	//int counter
	if(iFrames == 0)
	{
		timer.Reset();
		iFrames++;
	}

	iFrames++;
	if(iFrames == 101)
	{
		float fps = 100.0/timer.GetElapsedSeconds();

		printf("frame count:%f\n",fps);
		
		timer.Reset();
		iFrames = 1;
	}
}

void initFloorBatch()
{
	GLfloat alpha = 0.25f;
	floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(-20.0f, -0.41f, 20.0f);

		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 10.0f, 0.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(20.0f, -0.41f, 20.0f);

		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 10.0f, 10.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);

		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 0.0f, 10.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
	floorBatch.End();
}

void initMapBatch()
{/*
	int x = 500;
	int y = 445;
	int width = 300;
	int height = 155;*/
	GLfloat x = 0.3;
	GLfloat y = 0.2;
	GLfloat width = 10.0;
	GLfloat height = 10.0;
	GLfloat alpha = 0.25f;
	mapBatch.Begin(GL_TRIANGLE_FAN,4,1);
		mapBatch.Normal3f(0.0, 0.0f, 1.0f);
		mapBatch.MultiTexCoord2f(0,0.0f,0.0f);
		mapBatch.Vertex3f(x,y,-10.0f);

		mapBatch.Normal3f(0.0, 0.0f, 1.0f);
		mapBatch.MultiTexCoord2f(0,1.0f,0.0f);
		mapBatch.Vertex3f(x+width,y,-10.0f);
		
		mapBatch.Normal3f(0.0, 0.0f, 1.0f);
		mapBatch.MultiTexCoord2f(0,1.0f,1.0f);
		mapBatch.Vertex3f(x+width,y+height,-10.0f);

		mapBatch.Normal3f(0.0, 0.0f, 1.0f);
		mapBatch.MultiTexCoord2f(0,0.0,1.0);
		mapBatch.Vertex3f(x,y+height,-10.0f);	
	mapBatch.End();
}


void initBlurBuffer()
{
	pixelDataSize = screenWidth * screenHeight*3 * sizeof(unsigned int);
	void* data = (void*)malloc(pixelDataSize);
	memset(data,0x00,pixelDataSize);

	for(int i=0;i<6;i++)
	{
		glActiveTexture(GL_TEXTURE1+i);
		glBindTexture(GL_TEXTURE_2D,blurTextures[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,screenWidth,screenHeight,0,GL_RGB,GL_UNSIGNED_BYTE,data);
	}
	glGenBuffers(1,pixBufferObj);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,pixBufferObj[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER,pixelDataSize,pixelData,GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
}

void initFrameBuffer()
{
	glGenBuffers(1,&fboName);
	glBindBuffer(GL_DRAW_FRAMEBUFFER,fboName);

	//create depth buffer for FBO
	glGenBuffers(1,&depthFBOBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,depthFBOBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT32,screenWidth,screenHeight);

	//create a texture copy the scene
	glGenTextures(1,&fboTexture);
	glBindTexture(GL_TEXTURE_2D,fboTexture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,screenWidth,screenHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fboTexture,0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthFBOBuffer);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}


void Init()
{
	//init glew
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew error:%d\n",glewGetErrorString(err));
	}
	
	//init clear color
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	//init enable functions
	glEnable(GL_DEPTH_TEST);


	//init shader program
	shaderManager.InitializeStockShaders();
	blurShaderProg =  gltLoadShaderPairWithAttributes("blur.vs", "blur.fs", 2,
				GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "texCoord0");

	//init batch
	gltMakeTorus(torusBatch,0.5f,0.2f,35,35);
	gltGenerateOrtho2DMat(screenWidth, screenHeight, orthoMatrix, screenBatch);
	initFloorBatch();
	initMapBatch();

	//init texture
	glGenTextures(1, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	LoadBMPTexture("marble.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
	glGenTextures(6,blurTextures);


	//init buffer
	initBlurBuffer();
	//initFrameBuffer();
	
	gltCheckErrors();
}

void ShutDown()
{
	//bind all texture and buffers to nought
	glBindFramebuffer(GL_DRAW_BUFFER,0);
	glBindFramebuffer(GL_READ_BUFFER,0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER,0);

	for(int i=0;i<7;i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D,0);
	}

	//delete textures and buffers
	glDeleteTextures(6,blurTextures);
	glDeleteBuffers(1,pixBufferObj);
	glDeleteRenderbuffers(1,&depthFBOBuffer);
	glDeleteFramebuffers(1,&fboName);

}

void Reshape(int width,int height)
{
	glViewport(0,0,width,height);
	//init matrix and set perspective
	transformPipeline.SetMatrixStacks(modelviewMatrix,projectionMatrix);
	viewFrustum.SetPerspective(35.0f,(float)width/(float)height,1.0f,100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	modelviewMatrix.LoadIdentity();

	//reset pixel data when reshape
	screenHeight = height;
	screenWidth = width;
	
	gltGenerateOrtho2DMat(screenWidth,screenHeight,orthoMatrix,screenBatch);
	//set pixel data
	free(pixelData);
	pixelDataSize = screenWidth*screenHeight*3*sizeof(unsigned int);
	pixelData = (void*)malloc(pixelDataSize);
	//bind data to buffer
	glBindBuffer(GL_PIXEL_PACK_BUFFER,pixBufferObj[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER,pixelDataSize,pixelData,GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	gltCheckErrors();
}

float evaluatePosition()
{
	static CStopWatch timer;
	float seconds = timer.GetElapsedSeconds()*speedFactor;
	static float totalTime = 6.0f;
	static float halfTotalTime = totalTime*0.5;
	float xPosition = 0.0f;

	while(seconds >totalTime)
		seconds -= totalTime;

	if(seconds < halfTotalTime)
		xPosition = seconds - halfTotalTime*0.5;
	else
		xPosition = totalTime - seconds - halfTotalTime*0.5;

	return xPosition;
}

void drawSceneToFBO()
{
	void displayTorusAndFloor();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fboName);
	
	glDrawBuffers(1,fboBuffers);
	glViewport(0,0,screenWidth,screenHeight);
	
	displayTorusAndFloor();
	
	//reset fbo
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glDrawBuffers(1,windowBuffer);
}


void displayTorusAndFloor()
{
	//draw scene into frame buffer, and use it as a texture
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);

	M3DVector4f vLightTransform;
	m3dTransformVector4(vLightTransform,vLightPosition,mCamera);

	modelviewMatrix.PushMatrix();	
		cameraFrame.GetCameraMatrix(mCamera);
		modelviewMatrix.MultMatrix(mCamera);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]); // Marble

		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,
			transformPipeline.GetModelViewProjectionMatrix(), vWhite, 0);
		floorBatch.Draw();

		modelviewMatrix.PushMatrix();
			modelviewMatrix.Translate(0.0f,0.2f,-5.5f);
			modelviewMatrix.Translate(evaluatePosition(),0.0f,0.0f);
			shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
				modelviewMatrix.GetMatrix(),transformPipeline.GetProjectionMatrix(),
				vLightTransform,vGreen,0);
			torusBatch.Draw();
		modelviewMatrix.PopMatrix();
	modelviewMatrix.PopMatrix();
}

void displayMap()
{
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);

	modelviewMatrix.PushMatrix();	
		cameraFrame.GetCameraMatrix(mCamera);
		modelviewMatrix.MultMatrix(mCamera);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,GetBlurTarget(0)); 
		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,
			transformPipeline.GetModelViewProjectionMatrix(), 0);
		mapBatch.Draw();
	modelviewMatrix.PopMatrix();
}

void setBlurTexture()
{
	if(isUsePBO)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER,pixBufferObj[0]);
		glReadPixels(0,0,screenWidth,screenHeight,GL_RGB,GL_UNSIGNED_BYTE,NULL);
		glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
		
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixBufferObj[0]);
		glActiveTexture(GL_TEXTURE0+GetBlurTarget(0));
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,screenWidth,screenHeight,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
	}
	else
	{
		glReadPixels(0,0,screenWidth,screenHeight,GL_RGB,GL_UNSIGNED_BYTE,pixelData);
		glActiveTexture(GL_TEXTURE0+GetBlurTarget(0));
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,screenWidth,screenHeight,0,GL_RGB,GL_UNSIGNED_BYTE,pixelData);
	}
}

void setBlurProgram()
{
	glUseProgram(blurShaderProg);
	
	glUniformMatrix4fv(glGetUniformLocation(blurShaderProg,"mvpMatrix"),
		1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
	glUniform1i(glGetUniformLocation(blurShaderProg,"textureUnit0"),GetBlurTarget(0));
	glUniform1i(glGetUniformLocation(blurShaderProg,"textureUnit1"),GetBlurTarget(1));
	glUniform1i(glGetUniformLocation(blurShaderProg,"textureUnit2"),GetBlurTarget(2));
	glUniform1i(glGetUniformLocation(blurShaderProg,"textureUnit3"),GetBlurTarget(3));
	glUniform1i(glGetUniformLocation(blurShaderProg,"textureUnit4"),GetBlurTarget(4));
	glUniform1i(glGetUniformLocation(blurShaderProg,"textureUnit5"),GetBlurTarget(5));
}

void displayScreenWithBlur()
{
	projectionMatrix.PushMatrix();
		projectionMatrix.LoadIdentity();
		projectionMatrix.LoadMatrix(orthoMatrix);
		modelviewMatrix.PushMatrix();
			modelviewMatrix.LoadIdentity();
			glDisable(GL_DEPTH_TEST);
				setBlurProgram();
				screenBatch.Draw();
			glEnable(GL_DEPTH_TEST);
		modelviewMatrix.PopMatrix();
	projectionMatrix.PopMatrix();
}

void movingCamera()
{
	static CStopWatch cameraTimer;
	float fTime = cameraTimer.GetElapsedSeconds();
	cameraTimer.Reset(); 

	float linear = fTime * 3.0f;
	float angular = fTime * float(m3dDegToRad(30.0f));

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

void Keyboard(unsigned char key, int x, int y)
{ 
	static CStopWatch cameraTimer;
	float fTime = cameraTimer.GetElapsedSeconds();
	float linear = fTime * 12.0f;
	cameraTimer.Reset(); 

	// Alternate between PBOs and local memory when 'P' is pressed
	if(key == 'P' || key == 'p') 
		isUsePBO = (isUsePBO)? GL_FALSE : GL_TRUE;

	// Speed up movement
	if(key == '+')
	{
		speedFactor += linear/2;
		if(speedFactor > 6)
			speedFactor = 6;
	}

	// Slow down moement
	if(key == '-')
	{
		speedFactor -= linear/2;
		if(speedFactor < 0.5)
			speedFactor = 0.5;
	}
}

void InitElements()
{
	isMoveForward = false;
	isMoveBack = false;
	isRotateLeft = false;
	isRotateRight = false;
	isRotateUp = false;
	isRotateDown = false;
	screenWidth = 800;
	screenHeight = 600;
	speedFactor = 1.0;
	isFullScreen = false;
	isAnimated = true;
	isUsePBO = true;
	blurShaderProg = 0;
}

void Display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	displayTorusAndFloor();

	setBlurTexture();
	displayScreenWithBlur();
	displayMap();

	movingCamera();

	glutSwapBuffers();
	glutPostRedisplay();

	AdvanceBlurTarget();
	updateFrameCount();
}

int _tmain(int argc, char* argv[])
{
	InitElements();

	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(screenWidth,screenHeight);
	
	glutCreateWindow("pix buffers");

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutSpecialUpFunc(SpecialKeysUp);
	
	Init();

	glutMainLoop();

	ShutDown();

	return 0;
}

