// Terrain bounding box
uniform vec3 bbox_min;
uniform vec3 bbox_max;

varying vec3 vPixToLight;		// Vector of the current pixel to the light
varying vec3 vPixToEye;			// Vector of the current pixel to the eye
varying vec4 vPosition;
		
void main(void)
{
	gl_Position = ftransform();
	
	vPosition = gl_Vertex;
	vec3 vPositionNormalized = (gl_Vertex.xyz - bbox_min.xyz) / (bbox_max.xyz - bbox_min.xyz);
	gl_TexCoord[0].st = vPositionNormalized.xz;
	
	vPixToLight = -(gl_LightSource[0].position.xyz);		// Position (or direction) of light in the MV
	vPixToEye = -vec3(gl_ModelViewMatrix * gl_Vertex);		// Position of teh vertex in the MV
	
	// Multiplied by the light matrix, poisition of the vertex
	gl_TexCoord[1] = gl_TextureMatrix[0] * gl_Vertex;		
}
