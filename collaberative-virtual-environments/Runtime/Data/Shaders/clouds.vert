
/*varying vec3 vertex;
varying vec3 vertexMV;

varying vec2 texCoord;

void main(void)
{
	vertex = normalize(gl_Vertex.xyz);
	vertexMV = normalize(gl_ModelViewMatrix * vec4(-gl_Vertex.x, gl_Vertex.y, -gl_Vertex.z, 1.0)).xyz;	
	
	//texCoord = gl_MultiTexCoord0.st;
	texCoord = gl_TexCoord[0].xy;
	gl_Position = ftransform();
}
*/

varying vec3 position;

void main(void)
{
	position = gl_Vertex.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();	
}
