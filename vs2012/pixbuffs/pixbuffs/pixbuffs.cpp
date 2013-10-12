// pixbuffs.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <GLTools.h>
#include <GLTriangleBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>
#include <StopWatch.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#include <GL/glut.h>
#define STATIC_FREEGLUT
#endif

static GLfloat vGreen[] = {0.0f,1.0f,0.0f,1.0f};
static GLfloat vWhite[] = {1.0f,1.0f,1.0f,1.0f};
static GLfloat vLightPosition[] = {0.0f,3.0f,0.0f,1.0f};

GLsizei screenHeight;
GLsizei screenWidth;

GLboolean isFullScreen;
GLboolean isAnimated;

GLMatrixStack modelviewMatrix;
GLMatrixStack projectionMatrix;
M3DMatrix44f orthoMatrix;

GLFrustum viewFrustum;
GLFrame cameraFrame;
GLGeometryTransform transformPipeline;

GLTriangleBatch torusBatch;

GLuint textures[1];
GLuint blurTextures[6];
GLuint pixBufferObj[1];
GLuint curBlurTarget;
bool isUsePBO;
GLfloat speedFactor;
GLuint	blurShaderProg;
void *pixelData;
GLuint pixelDataSize;


void AdvanceBlurTarget()	{curBlurTarget = (curBlurTarget+1) % 6;}
int GetBlurTarget(int i) {return ((curBlurTarget + 5-i) + 1)%6;}

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

void Init()
{
	//glew init
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew error:%d\n",glewGetErrorString(err));
	}
	
	//init clear color
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	//init enable functions
	glEnable(GL_DEPTH_TEST);

	//init batch
	gltMakeTorus(torusBatch,1.0,0.5,32,32);

	//init shader program
	blurShaderProg = gltLoadShaderPairSrcWithAttributes("blur.vs","blur.fs",
		2,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_COLOR,"vColor");

	//init texture
	glGenTextures(6,blurTextures);

	pixelDataSize = screenWidth * screenHeight*3 * sizeof(unsigned int);
}



int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

