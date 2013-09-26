#include "stdafx.h"
#include "ReaderClass.h"

GLFrame cameraFrame;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeLine;
GLFrustum viewFrustum;


GLShaderManager shaderManager;
GLBatch fbxBatch;
GLuint fbxTexture;

Reader rTest;

GLuint LoadBMPTexture(const char *szFileName)	
{
	GLbyte *pBits;
	GLint iWidth, iHeight;
	GLuint texture;

	pBits = gltReadBMPBits(szFileName, &iWidth, &iHeight);
	if(pBits == NULL)
		return false;

	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	// Set Wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Do I need to generate mipmaps?

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pBits);
	return texture;
}


void Init()
{
	glClearColor(0.3,0.3,0.3,1.0);

	rTest.InitReader("Attack_Big_Center.FBX");
	//init openGL functions
	glEnable(GL_TEXTURE_2D);										// Enable Texture Mapping ( NEW )
	glShadeModel(GL_SMOOTH);										// Enable Smooth Shading
	glClearDepth(1.0f);												// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);										// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);											// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);				// Really Nice Perspective Calculations
	//init as usual

	shaderManager.InitializeStockShaders();
	//enable
	glEnable(GL_DEPTH_TEST);
	//move camera
	cameraFrame.MoveForward(-1000.0);
	//load texture
	fbxTexture = LoadBMPTexture("T_Sumo_Enemy_D.bmp");
}

void Reshape(int w,int h)
{
	if(h == 0) h = 1;
	glViewport(0,0,w,h);

	viewFrustum.SetPerspective(35.0,(float) w /(float) h,1.0,2000.0);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeLine.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	static CStopWatch timer;
	GLfloat yRot = timer.GetElapsedSeconds()*20.0f;

	GLfloat vWhite[] = {1.0f,1.0f,1.0f,1.0f};
	GLfloat vLightPos[] = {0.0f,2.0f,2.0f};
	GLfloat vAmbient[] = {0.3f,0.3f,1.0f,1.0f};
	modelViewMatrix.PushMatrix();
		//move to camera view
		M3DMatrix44f mCamera;
		cameraFrame.GetCameraMatrix(mCamera);
		modelViewMatrix.MultMatrix(mCamera);

		modelViewMatrix.PushMatrix();
		modelViewMatrix.Rotate(yRot,1.0,1.0,1.0);
		glBindTexture(GL_TEXTURE_2D,fbxTexture);
		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, 
                                     transformPipeLine.GetModelViewMatrix(),
                                     transformPipeLine.GetProjectionMatrix(), 
                                     vLightPos, vWhite, 0);
		//*/shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeLine.GetModelViewProjectionMatrix(),vWhite);
		modelViewMatrix.Scale(0.05,0.05,0.05);
		rTest.DrawReader();

		modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void ShutdownRC()
{

}

void SpecialKeys(int key, int x, int y)
{
	float linear = 0.1f;
	float angular = float(m3dDegToRad(5.0f));
	
	if(key == GLUT_KEY_UP)
		cameraFrame.MoveForward(linear);
	
	if(key == GLUT_KEY_DOWN)
		cameraFrame.MoveForward(-linear);
	
	if(key == GLUT_KEY_LEFT)
		cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
	
	if(key == GLUT_KEY_RIGHT)
		cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);		
	if(key ==GLUT_KEY_END) exit(0);
}

int main(int argc, char* argv[])
{
    // Destroy the SDK manager and all the other objects it was handling.

	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800,600);
	glutCreateWindow("test");
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutSpecialFunc(SpecialKeys);
	GLenum  err= glewInit();
	if(err != GLEW_OK)
	{
		fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
		return 1;
	}

	Init();
	glutMainLoop();

	ShutdownRC();

	return 0;
}