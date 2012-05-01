

varying vec4 vPixToLightTBN[1];				// Vector of the current pixel in the light
varying vec3 vPixToEyeTBN;					// Vector of the current pixel to the eye
varying vec3 vVertexMV;
varying vec3 vNormalMV;
varying vec3 vPixToLightMV;
varying vec3 vLightDirMV;

// SHADOW MAPPING //
uniform int enable_shadow_mapping;
////////////////////


#define MODE_PHONG		0
#define MODE_BUMP		1
#define MODE_PARALLAX	2
#define MODE_RELIEF		3
uniform int mode;

#define LIGHT_DIRECTIONAL		0.0
#define LIGHT_OMNIDIRECTIONAL	1.0
#define LIGHT_SPOT				2.0
				
void main(void)
{

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec3 vTangent = gl_MultiTexCoord1.xyz;
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * vTangent);
	vec3 b = cross(n, t);
	
	vNormalMV = n;
	
	vec4 vLightPosMV = gl_LightSource[0].position;		// Position (or direction) of light in the MV
	vVertexMV = vec3(gl_ModelViewMatrix * gl_Vertex);	// Position of the vertex in MV
	
	vec3 tmpVec;



	if(vLightPosMV.w == LIGHT_DIRECTIONAL)
		tmpVec = -vLightPosMV.xyz;					// directional light
	else
		tmpVec = vLightPosMV.xyz - vVertexMV.xyz;	// point light

	vPixToLightMV = tmpVec;
/*
	if(mode == MODE_PHONG)
	{
		vPixToLightTBN[0].xyz = tmpVec.xyz;
		vPixToLightTBN[0].w = vLightPosMV.w;	// point or directional
		
		vPixToEyeTBN = -vVertexMV;
	}
	else*/
	{
		// Position or direction of light
		vPixToLightTBN[0].x = dot(tmpVec, t);
		vPixToLightTBN[0].y = dot(tmpVec, b);
		vPixToLightTBN[0].z = dot(tmpVec, n);
		vPixToLightTBN[0].w = vLightPosMV.w;	// point or directional
			
		// vector for
		tmpVec = -vVertexMV;
		vPixToEyeTBN.x = dot(tmpVec, t);
		vPixToEyeTBN.y = dot(tmpVec, b);
		vPixToEyeTBN.z = dot(tmpVec, n);
	}
	
	
	
	if(length(gl_LightSource[0].spotDirection) > 0.001)
	{
		// Spot Light
		vLightDirMV = normalize(gl_LightSource[0].spotDirection);
		vPixToLightTBN[0].w = LIGHT_SPOT;
	}
	else
	{
		// No a spot light
		vLightDirMV = gl_LightSource[0].spotDirection;
	}
	
	if(enable_shadow_mapping != 0) {
		// pos has undergone transformations + the camera
		vec4 pos = gl_ModelViewMatrix * gl_Vertex;
		// multiplied by the inverse matrix of the camera: pos has only undergoes transformation
		pos = gl_TextureMatrix[0] * pos;
		// is multiplied by the matrix of light: Vertex position in the frame of the light
		gl_TexCoord[1] = gl_TextureMatrix[1] * pos;
	}
}
