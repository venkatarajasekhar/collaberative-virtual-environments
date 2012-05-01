varying vec3 vPixToLight;
varying vec3 vPixToEye;

void main(void)
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}
