// fbodrawbuffers.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLMatrixStack.h>
#include <GLFrustum.h>
#include <GLFrame.h>
#include <GLGeometryTransform.h>
#include <GLTriangleBatch.h>
#include <GLBatch.h>
#include <StopWatch.h>

GLShaderManager shaderManger;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLFrame cameraFrame;
GLGeometryTransform transformPipeline;

GLTriangleBatch torusBatch;
GLBatch floorBatch;

GLuint floorTexture;
GLuint fbo;
GLuint depthBufferForFbo;
GLuint renderBufferForFbo;




int _tmain(int argc, char* argv[])
{
	return 0;
}

