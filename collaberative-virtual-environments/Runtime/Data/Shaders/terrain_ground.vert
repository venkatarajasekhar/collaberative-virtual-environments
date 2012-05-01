

varying vec4 vPixToLightTBN;		// Pixel to Light vector
varying vec3 vPixToEyeTBN;			// Pixel to Eye vector
varying vec3 vPosition;
varying vec3 vPositionNormalized;

// Bounding Box for terrain
uniform vec3 bbox_min;
uniform vec3 bbox_max;

				
void main(void)
{
	gl_Position = ftransform();
	
	// Position
	vPosition = gl_Vertex.xyz;
	
	// Position of vertex on terrain between 0 and 1
	vPositionNormalized = (gl_Vertex.xyz - bbox_min.xyz) / (bbox_max.xyz - bbox_min.xyz);
	
	// Texture coords
	gl_TexCoord[0].st = vPositionNormalized.xz;
	
	
	// Calculate space TBN
	vec3 vTangent = gl_MultiTexCoord0.xyz;
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * vTangent);
	vec3 b = cross(t, n);
	
	vec4 vLightPosMV = gl_LightSource[0].position;			// Position (or direction) of light in the MV
	vec3 vVertexMV = vec3(gl_ModelViewMatrix * gl_Vertex);	// Position in the MV
	
	vec3 tmpVec = -vLightPosMV.xyz;							// Light direction

	// Light vector in space TBN
	vPixToLightTBN.x = dot(tmpVec, t);
	vPixToLightTBN.y = dot(tmpVec, b);
	vPixToLightTBN.z = dot(tmpVec, n);
	vPixToLightTBN.w = vLightPosMV.w;	// Point or directional
	
	// Vector space for TBN
	tmpVec = -vVertexMV;
	vPixToEyeTBN.x = dot(tmpVec, t);
	vPixToEyeTBN.y = dot(tmpVec, b);
	vPixToEyeTBN.z = dot(tmpVec, n);
	
	// multiply by light matrix, vertex position in light coord
	gl_TexCoord[1] = gl_TextureMatrix[0] * gl_Vertex;	
	
}





