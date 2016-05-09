// MS3DModel.h: interface for the MS3DModel class.
// Programmer:	Joe Waller
// Description:	Class will hold all the information of a .ms3d file
//				for use with OpenGL.  .ms3d file specifications found
//				in the MS3D SDK.  These data structures are nearly identical
//				to those used in the SDK.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MS3DMODEL_H__58596192_08D4_414E_BFC4_0AC88578B4B1__INCLUDED_)
#define AFX_MS3DMODEL_H__58596192_08D4_414E_BFC4_0AC88578B4B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl/glut.h>

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef word
typedef unsigned short word;
#endif

#ifdef _MSC_VER
#	pragma pack(push, packing)
#	pragma pack(1)
#	define PACK
#endif

class MS3DModel  
{
public:
	bool LoadMS3DModel(const char* filename);
	void LoadTextures();
	MS3DModel();
	virtual ~MS3DModel();
	void Render();
	
	struct MS3DHeader
	{
		char	id[10];
		int		version;
	};

	struct MS3DVertex 
	{
		byte	flags;
		float	vertex[3];
		char	boneId;
		byte	referenceCount;
	};
	
	struct MS3DTriangle
	{
		word	flags;
		word	vertexIndices[3];
		float	vertexNormals[3][3];
		float	s[3];
		float	t[3];
		byte	smoothingGroup;
		byte	groupIndex;
	};

	struct MS3DGroup
	{
		byte	flags;
		char	name[32];
		word	numtriangles;
		word*	triangleIndices;	// this will be triangleIndices[numTriangles]
		char	materialIndex;
	};

	struct MS3DMaterial
	{
		char	name[32];
		float	ambient[4];
		float	diffuse[4];
		float	specular[4];
		float	emissive[4];
		float	shininess;
		float	transparency;
		char	mode;
		char	texture[128];
		char	alphamap[128];
		GLuint	nTexture;
	};

	struct MS3DKeyframeRot
	{
		float	time;
		float	rotation[3];
	};

	struct MS3DKeyframePos
	{
		float	time;
		float	position[3];
	};

	struct MS3DJoint
	{
		byte	flags;
		char	name[32];
		char	parentsName[32];
		float	rotation[3];
		float	position[3];
		word	numKeyFramesRot;
		word	numKeyFramesTrans;
		MS3DKeyframeRot*	keyFramesRot;		// this will be keyFrameRot[numKeyFramesRot]
		MS3DKeyframePos*	keyFramesTrans;		// this will be keyFrameTrans[numKeyFramesTrans]
	};

private:
	MS3DHeader		header;
	word			nNumVertices;
	MS3DVertex*		pVertices;
	word			nNumTriangles;
	MS3DTriangle*	pTriangles;
	word			nNumGroups;
	MS3DGroup*		pGroups;
	word			nNumMaterials;
	MS3DMaterial*	pMaterials;
	float			fAnimationFPS;
	float			fCurrentTime;
	int				iTotalFrames;
	word			nNumJoints;
};

#endif // !defined(AFX_MS3DMODEL_H__58596192_08D4_414E_BFC4_0AC88578B4B1__INCLUDED_)
