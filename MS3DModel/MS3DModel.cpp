// MS3DModel.cpp: implementation of the MS3DModel class.
//
//////////////////////////////////////////////////////////////////////

#include "MS3DModel.h"
#include <windows.h>
#include <iostream>
#include <fstream.h>
#include <string.h>
#include <gl\glaux.h>
#include <gl\glut.h>

#pragma comment(lib, "glaux.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MS3DModel::MS3DModel()
{
	nNumGroups = 0;
	pGroups = NULL;
	nNumMaterials = 0;
	pMaterials = NULL;
	nNumVertices = 0;
	pVertices = NULL;
	nNumTriangles = 0;
	pTriangles = NULL;
}

MS3DModel::~MS3DModel()
{
	int i;

	for (i=0; i<nNumGroups; i++ )
		delete[] pGroups[i].triangleIndices;

	nNumGroups = 0;
	if (pGroups != NULL)
	{
		delete[] pGroups;
		pGroups = NULL;
	}

	nNumMaterials = 0;
	if (pMaterials != NULL)
	{
		delete[] pMaterials;
		pMaterials = NULL;
	}

	nNumTriangles = 0;
	if (pTriangles != NULL)
	{
		delete[] pTriangles;
		pTriangles = NULL;
	}

	nNumVertices = 0;
	if (pVertices != NULL)
	{
		delete[] pVertices;
		pVertices = NULL;
	}
}

bool MS3DModel::LoadMS3DModel(const char* filename)
{
	int i;

	ifstream input_file(filename, (ios::binary | ios::in));
	if(!input_file)
		return false;				// file does not exist

	// Originally I tried to read the data from the file and put it
	// directly into the data structures.  This didn't work out, so instead
	// I have read the data into a buffer in its entirety and plan to
	// put the data into the data structures from that buffer

	byte *buffer = new byte[500];
	input_file.read(buffer, 14);
	MS3DHeader *ptrHeader = (MS3DHeader*)buffer;
	memcpy(header.id, ptrHeader->id, 10);
	header.version = ptrHeader->version;

	if(strncmp(header.id, "MS3D000000", 10) != 0)
		return false;				// not a valid model file
	if((header.version < 3) || (header.version > 4))
		return false;				// version not supported

	// - So far so good -----------------------------------------------------

	input_file.read(buffer, 2);
	nNumVertices = *(word*)buffer;		// we now have the number of vertices

	MS3DVertex *ptrVertex;
	pVertices = new MS3DVertex[nNumVertices];
	for(i=0; i<nNumVertices; i++)
	{
		input_file.read(buffer, 15);
		ptrVertex = (MS3DVertex*)buffer;
		pVertices[i] = *ptrVertex;
	}									// stored all the vertices

	input_file.read(buffer, 2);
	nNumTriangles = *(word*)buffer;		// stored number of triangles

	MS3DTriangle *ptrTriangle;
	pTriangles = new MS3DTriangle[nNumTriangles];
	for(i=0; i<nNumTriangles; i++)
	{
		input_file.read(buffer, 70);
		ptrTriangle = (MS3DTriangle*)buffer;
		pTriangles[i] = *ptrTriangle;
		pTriangles[i].t[0] = 1.0 - pTriangles[i].t[0];
		pTriangles[i].t[1] = 1.0 - pTriangles[i].t[1];
		pTriangles[i].t[2] = 1.0 - pTriangles[i].t[2];
	}									// stored triangle data

	input_file.read(buffer, 2);
	nNumGroups = *(word*)buffer;

	pGroups = new MS3DGroup[nNumGroups];
	for(i=0; i<nNumGroups; i++)
	{
		input_file.read(buffer, 1);
		pGroups[i].flags = *(byte*)buffer;
		input_file.read(buffer, 32);
		memcpy(pGroups[i].name, buffer, 32);
		input_file.read(buffer, 2);
		pGroups[i].numtriangles = *(word*)buffer;
		pGroups[i].triangleIndices = new word[pGroups[i].numtriangles];
		for(int j=0; j<pGroups[i].numtriangles; j++)
		{
			input_file.read(buffer, 2);
			pGroups[i].triangleIndices[j] = *(word*)buffer;
		}
		input_file.read(buffer, 1);
		pGroups[i].materialIndex = *buffer;
	}											// stored the group data

	input_file.read(buffer, 2);
	nNumMaterials = *(word*)buffer;

	pMaterials = new MS3DMaterial[nNumMaterials];
	MS3DMaterial *ptrMaterial;
	for(i=0; i<nNumMaterials; i++)
	{
		input_file.read(buffer, 361);
		ptrMaterial = (MS3DMaterial*)buffer;
		pMaterials[i] = *ptrMaterial;
		pMaterials[i].nTexture = NULL;

		int pos;
		char buff[128];
		int pathLen = strlen(pMaterials[i].texture);
		char strBuffer[128];
		for(int j=pathLen-1; j>=0; j--)
		{
			cout << "Testing " << pMaterials[i].texture[j] << endl;
			if((pMaterials[i].texture[j] == '/') || (pMaterials[i].texture[j] == '\\'))
			{
				pos = j;
				break;
			}
		}
		buff[0] = '.';
		buff[1] = '/';
		buff[2] = 't';
		buff[3] = 'e';
		buff[4] = 'x';
		buff[5] = 't';
		buff[6] = 'u';
		buff[7] = 'r';
		buff[8] = 'e';
		buff[9] = 's';
		buff[10] = '/';
		cout << pos << endl;
		for(int k=0; k<pathLen-pos; k++)
		{
			buff[11+k] = pMaterials[i].texture[pos+k+1];
		}

		buff[pathLen-pos+10] = NULL;
		strcpy(pMaterials[i].texture, buff);
		cout << pMaterials[i].texture << endl;
	}

	// There is more info in the data file.  However, most of this is animation
	// data and is not something I am prepared to mess with.  Therefore, since
	// parcing the file up to this point has taken a very long time, I cease work
	// on file input for the time being

	input_file.close();
	delete buffer;

	LoadTextures();

	return true;
}

void MS3DModel::LoadTextures()
{
	for(int i=0; i<nNumMaterials; i++)
	{
		if(strlen(pMaterials[i].texture) > 0)
		{
			AUX_RGBImageRec *ptrBitmap;
			GLuint texture;

			FILE *file = NULL;
			if(!pMaterials[i].texture)
			{
				ptrBitmap = NULL;
			}
			file = fopen(pMaterials[i].texture, "r");
			if(file)
			{
				fclose(file);
				ptrBitmap = auxDIBImageLoad(pMaterials[i].texture);
			}
			else
			{
				ptrBitmap = NULL;
			}

			if(ptrBitmap != NULL && ptrBitmap->data != NULL)
			{
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, ptrBitmap->sizeX, ptrBitmap->sizeY,
						     0, GL_RGB, GL_UNSIGNED_BYTE, ptrBitmap->data);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				free(ptrBitmap->data);
				free(ptrBitmap);
			}
			pMaterials[i].nTexture = texture;
		}
		else
			pMaterials[i].nTexture = 0;
	}
}

void MS3DModel::Render()
{
	GLboolean texEnabled = glIsEnabled( GL_TEXTURE_2D );
	for(int i=0; i<nNumGroups; i++)
	{
		// Set up the material properties
		int j = pGroups[i].materialIndex;
		if(j >= 0)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, pMaterials[j].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, pMaterials[j].diffuse);
			glMaterialfv(GL_FRONT, GL_EMISSION, pMaterials[j].emissive);
			glMaterialf(GL_FRONT, GL_SHININESS, pMaterials[j].shininess);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pMaterials[j].specular);
			if(pMaterials[j].nTexture != 0)
			{
				glBindTexture(GL_TEXTURE_2D, pMaterials[j].nTexture);
				glEnable(GL_TEXTURE_2D);
			}
			else
				glDisable(GL_TEXTURE_2D);
		}
		else
			glDisable(GL_TEXTURE_2D);

		// Start rendering the vertices
		glBegin(GL_TRIANGLES);
			for(int k=0; k<pGroups[i].numtriangles; k++)
			{
				int m = pGroups[i].triangleIndices[k];
				MS3DTriangle *ptrTriangle = &pTriangles[m];
				for(int p=0; p<3; p++)
				{
					int q = ptrTriangle->vertexIndices[p];
					glNormal3fv(ptrTriangle->vertexNormals[p]);
					glTexCoord2f(ptrTriangle->s[p], ptrTriangle->t[p]);
					glVertex3fv(pVertices[q].vertex);
				}
			}
		glEnd();
	}
	if(texEnabled)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}
