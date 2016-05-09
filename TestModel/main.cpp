
//	Programmer:	Joe Waller
//	Module:		main.cpp
//	Objective:	This is not much different than rendering a cube as far
//				as drawing geometric shapes is concerned.  My goal in this
//				program was to rotate about a camera position as to give
//				the effect of turning in place.

#include<GL/glut.h>
#include<windows.h>
#include<GL/glaux.h>
#include<math.h>
#include "MS3DModel.h"
#include<fstream.h>
#include<stdio.h>	

#define PI_OVER_180		0.0174532925

// Global Variables ****************************************************

bool keys[256];
GLfloat y_rotation = 0.0;
GLfloat x_translation = 0.0;
GLfloat z_translation = 0.0;
GLfloat y_translation = 0.0;
GLfloat	rotate_speed = 0.3;
GLfloat jump_speed = 0.035;
GLfloat translate_speed = 0.1;
bool jump_state = false;
bool up_state = true;
MS3DModel *myModel = NULL;

// *********************************************************************

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(h == 0)				// take care of divide by zero problem
		h = 1;
	gluPerspective(50.0, (float)w/(float)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void IdleFunction(void)
{
	if(keys[GLUT_KEY_UP] == true)
	{
		z_translation -= (float)cos(y_rotation*PI_OVER_180) * translate_speed;
		x_translation -= (float)sin(y_rotation*PI_OVER_180) * translate_speed;
	}
	if(keys[GLUT_KEY_DOWN] == true)
	{
		z_translation += (float)cos(y_rotation*PI_OVER_180) * translate_speed;
		x_translation += (float)sin(y_rotation*PI_OVER_180) * translate_speed;
	}
	if(keys[GLUT_KEY_RIGHT] == true)
		y_rotation -= rotate_speed;
	if(keys[GLUT_KEY_LEFT] == true)
		y_rotation += rotate_speed;
	if((jump_state == false) && (keys[GLUT_KEY_PAGE_DOWN] == true))
	{
		jump_state = true;
	}
	if(jump_state == true)
	{
		if(up_state == true)
			y_translation += jump_speed;
		if(up_state == false)
		{
			y_translation -= jump_speed;
			if(y_translation <= 0.0)
			{
				jump_state = false;
				up_state = true;
				y_translation = 0.0;
			}
		}
		if(y_translation > 4.0)
			up_state = false;
	}
	glutPostRedisplay();
}

void NormalKeys(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);		// exit it the program NOW!!!
	}
}

void SpecialKeys(int key, int x, int y)
{
	keys[key] = true;
}

void SpecialKeysUp(int key, int x, int y)
{
	keys[key] = false;
}

void Initialize(void)
{
	myModel->LoadTextures();
	keys[GLUT_KEY_UP] = false;
	keys[GLUT_KEY_DOWN] = false;
	keys[GLUT_KEY_RIGHT] = false;
	keys[GLUT_KEY_LEFT] = false;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glutIgnoreKeyRepeat(1);
	
	GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.0 };
	GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();

	glRotatef((360.0 - y_rotation), 0.0, 1.0, 0.0);
	glTranslatef(-x_translation, -y_translation-10, -z_translation);
	myModel->Render();

	glPopMatrix();

	glutSwapBuffers();
}

bool GetFullScreen()
{
	return (MessageBox(NULL, "Would you like to run in full screen mode?", "Question", 
			MB_ICONQUESTION | MB_YESNO) == IDYES);
}

int main(int argc, char** argv)
{
	bool fullscreen = GetFullScreen();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	myModel = new MS3DModel();

	if(fullscreen == true)
	{
		glutGameModeString("640x480:16");
		if(glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		{
			translate_speed = 0.5;
			rotate_speed = 1.2;
			jump_speed = 0.3;
			glutEnterGameMode();
		}
		else
		{
			MessageBox(NULL, "Cannot enter fullscreen mode.  Switching to windowed mode.",
					   "Error", MB_OK);
			fullscreen = false;
		}
	}
	if(fullscreen == false)
	{
		glutInitWindowSize(600, 400);
		glutInitWindowPosition(200, 200);
		glutCreateWindow("3D World");
	}
	if(!myModel->LoadMS3DModel("models/zombie02.ms3d"))
		MessageBox(NULL, "Error Loading Model!", "Error!", MB_OK);

	Initialize();

	glutDisplayFunc(Display);
	glutKeyboardFunc(NormalKeys);
	glutReshapeFunc(Reshape);
	glutIdleFunc(IdleFunction);
	glutSpecialFunc(SpecialKeys);
	glutSpecialUpFunc(SpecialKeysUp);

	glutMainLoop();

	return 0;
}