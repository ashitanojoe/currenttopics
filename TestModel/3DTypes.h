

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
