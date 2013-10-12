#include "stdafx.h"
#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include <StopWatch.h>

#define SPHERE_NUM 20
GLFrame spheres[SPHERE_NUM];

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLFrame cameraFrame;
GLGeometryTransform transformPipeline;

GLBatch cube;
GLTriangleBatch torus;
GLBatch ground;
GLuint texture[3];

bool LoadTexture(const char *file,GLenum minFilter,GLenum magFilter,GLenum wrapMode)
{
    GLbyte *pBits;			//to read file
	int width,height,components;		//texture's w and h
	GLenum eformat;

	pBits = gltReadTGABits(file,&width,&height,&components,&eformat);
	if(pBits == NULL) return false;			//cannot read file

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrapMode);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrapMode);			//set wrap mode
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);		//set filtermode
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(GL_TEXTURE_2D,0,GL_COMPRESSED_RGB,width,height,0,eformat,GL_UNSIGNED_BYTE,pBits);

	free(pBits);

	//use with mip
	if(minFilter == GL_LINEAR_MIPMAP_LINEAR||
		minFilter == GL_LINEAR_MIPMAP_NEAREST||
		minFilter == GL_NEAREST_MIPMAP_LINEAR||
		minFilter == GL_NEAREST_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

void draw(float yRot,M3DMatrix44f mCamera)
{
	static GLfloat vWhit[] = {1.0,1.0,1.0,1.0};
	static GLfloat vLightPos[] = {0.0,3.0,0.0,1.0};

	M3DVector4f vLightTrans;
	m3dTransformVector4(vLightTrans,vLightPos,mCamera);
	modelViewMatrix.PushMatrix();
		modelViewMatrix.Translatev(vLightPos);
		cube.Draw();
	modelViewMatrix.PopMatrix();

	int i;
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	for(i = 0;i<SPHERE_NUM;i++)
	{
		modelViewMatrix.PushMatrix();
		//modelViewMatrix.MultMatrix(spheres);
		cube.Draw();
		modelViewMatrix.PopMatrix();
	}
	modelViewMatrix.Translate(0.0,0.2,-0.25);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Rotate(yRot,0.0,1.0,0.0);
	glBindTexture(GL_TEXTURE_2D,texture[2]);
	torus.Draw();
	modelViewMatrix.PopMatrix();
	
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0,0.0,0.0,1.0);

	gltMakeCube(cube,0.5f);
	gltMakeTorus(torus,1.0,0.6,25,20);
	//draw ground
	GLfloat texsize = 10.0;
	ground.Begin(GL_TRIANGLE_FAN,4,1);
	ground.MultiTexCoord2f(0,0.0,0.0);
	ground.Vertex3f(-20.0,-1.0,-10.0);

	ground.MultiTexCoord2f(0,texsize,0.0);
	ground.Vertex3f(-20.0,-1.0,10.0);

	ground.MultiTexCoord2f(0,texsize,texsize);
	ground.Vertex3f(20.0,-1.0,10.0);

	ground.MultiTexCoord2f(0,0.0,texsize);
	ground.Vertex3f(20.0,-1.0,-10.0);
	ground.End();
	
	//load 3 textures
	glGenTextures(3,texture);
	
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	LoadTexture("Marble.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	LoadTexture("marslike.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D,texture[2]);
	LoadTexture("Moonlike.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE);
	
	//draw SPHERE_NUM spheres
	int i;
	GLfloat x,z;
	for(i =0;i<SPHERE_NUM;i++)
	{
		x= ((GLfloat)(rand()%200-400))/10;
		z = ((GLfloat)(rand()%100-200))/10;
		spheres[i].SetOrigin(x,0,z);
	}
}

//dif
void reshape(int width,int height)
{
	glViewport(0,0,width,height);
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
	viewFrustum.SetPerspective(35.0,(float)width/(float)height,1.0,100.0);

	projectionMatrix.LoadIdentity();
	modelViewMatrix.LoadIdentity();
}

void display()
{
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds()*60.0;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	modelViewMatrix.PushMatrix();
		M3DMatrix44f mCamera;
		cameraFrame.GetCameraMatrix(mCamera);
		modelViewMatrix.MultMatrix(mCamera);
		modelViewMatrix.PushMatrix();
		modelViewMatrix.Translate(0.0,-0.8,0.0);

		draw(yRot,mCamera);

		modelViewMatrix.PopMatrix();
		glBindTexture(GL_TEXTURE_2D,texture[0]);
		ground.Draw();

		draw(yRot,mCamera);
		
		modelViewMatrix.PopMatrix();
		glutSwapBuffers();
		glutPostRedisplay();
		
}

void keyboard(unsigned char key,int x,int y)
{
	float linear = 0.1f;
	float angleY = float(m3dDegToRad(5.0));
	float angleX = float(m3dDegToRad(5.0));

	switch (key)
	{
	case GLUT_KEY_UP:
		cameraFrame.MoveForward(linear);
		break;
	case GLUT_KEY_DOWN:
		cameraFrame.MoveForward(-linear);
		break;
	case GLUT_KEY_RIGHT:
		cameraFrame.RotateWorld(angleY,0.0,1.0,0.0);
		break;
	case GLUT_KEY_LEFT:
		cameraFrame.RotateWorld(-angleY,0.0,1.0,0.0);
		break;
	case GLUT_KEY_PAGE_UP:
		cameraFrame.RotateLocalX(angleX);
		break;
	case GLUT_KEY_PAGE_DOWN:
		cameraFrame.RotateLocalX(angleY);
		break;
	case GLUT_KEY_F1:
		exit(0);
	default:
		break;
	}
}

int main(int argc,char *argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(800,600);
	glutCreateWindow("cubeworld");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	init();
	glutMainLoop();
	return 0;
}

