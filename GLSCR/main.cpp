
#include <windows.h>
#include <scrnsave.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "MS3DModel.h"

#define TIMER			1
#define PI_OVER_180		0.0174532925

int Width, Height;
MS3DModel *UserModel = NULL;
MS3DModel *zombie = NULL;
MS3DModel *ground = NULL;
GLdouble rotation = 0.0;
GLdouble radius = 15.0;

struct Camera
{
	GLdouble eyex;	// the camera position
	GLdouble eyey;
	GLdouble eyez;

	GLdouble centerx; // the direction the camera is pointing
	GLdouble centery;
	GLdouble centerz;

	GLdouble upx; // the up vector for the camera
	GLdouble upy;
	GLdouble upz;
} theCamera;

void GetConfig();
void SetConfig();
static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC);
static void KillGL(HWND hWnd, HDC hDC, HGLRC hRC);
void SetupScreen(int w, int h);
void OnTimer(HDC hDC);

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;
	static HGLRC hRC;
	static RECT rect;

	switch (message)
	{
	case WM_CREATE:
		UserModel = new MS3DModel();
		zombie = new MS3DModel();
		ground = new MS3DModel();
		GetClientRect(hWnd, &rect);
		Height = rect.bottom;
		Width = rect.right;
		InitGL(hWnd, hDC, hRC);				// sets up as OpenGL Window
		SetupScreen(Width, Height);
		SetTimer(hWnd, TIMER, 10, NULL);
		if (!UserModel->LoadMS3DModel("C:/Windows/models/jeep1.ms3d"))
			MessageBox(NULL, "Error!", "Error Loading Model!", MB_OK);
		if (!zombie->LoadMS3DModel("C:/Windows/models/tree.ms3d"))
			MessageBox(NULL, "Error!", "Error Loading Model!", MB_OK);
		if (!ground->LoadMS3DModel("C:/Windows/models/ground.ms3d"))
			MessageBox(NULL, "Error!", "Error Loading Model!", MB_OK);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, TIMER);
		KillGL(hWnd, hDC, hRC);
		return 0;

	case WM_TIMER:
		OnTimer(hDC);
		return 0;
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	hDC = GetDC(hWnd);

	int buff = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, buff, &pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	UserModel->LoadTextures();
	zombie->LoadTextures();
	ground->LoadTextures();

	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ambient[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat spec[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

static void KillGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					EndDialog(hDlg, LOWORD(wParam) == IDOK);
					return TRUE;
				case IDC_BUTTON_CHANGE:
			}
	}

	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}

void SetupScreen(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(50.0, (float)w/(float)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	theCamera.centerx = 0.0;
	theCamera.centery = 5.0;
	theCamera.centerz = 50.0;
	theCamera.eyex = 0.0;
	theCamera.eyey = 0.0;
	theCamera.eyez = 0.0;
	theCamera.upx = 0.0;
	theCamera.upy = 1.0;
	theCamera.upz = 0.0;

	gluLookAt(theCamera.centerx, theCamera.centery, theCamera.centerz,
			  theCamera.eyex, theCamera.eyey, theCamera.eyez,
			  theCamera.upx, theCamera.upy, theCamera.upz);

//	gluLookAt(0.0, 30.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glClearColor(0.0, 0.0, 1.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	glColor3f(0.1f, 1.0f, 0.3f);
}

void OnTimer(HDC hDC)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(rotation >= 360.0)
		rotation = 0.0;
	else
		rotation += 0.5;

	theCamera.centerx = (GLdouble)(radius*cos(rotation*PI_OVER_180));
	theCamera.centerz = (GLdouble)(radius*sin(rotation*PI_OVER_180));

	glPushMatrix();
	gluLookAt(theCamera.centerx, theCamera.centery, theCamera.centerz,
			  theCamera.eyex, theCamera.eyey, theCamera.eyez,
			  theCamera.upx, theCamera.upy, theCamera.upz);

	glPushMatrix();
	glScalef(30.0, 30.0, 30.0);
	ground->Render();
	glPopMatrix();
	
	UserModel->Render();
	for(int k=0; k<10; k++)
	{
		glPushMatrix();
		glRotatef(36*k, 0.0, 1.0, 0.0);
		glTranslatef(60.0, 0.0, 0.0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		glScalef(4.0, 4.0, 4.0);
		zombie->Render();
		glPopMatrix();
	}

	glFinish();
	SwapBuffers(hDC);
	glPopMatrix();
}
