varying vec4 vPixToLightTBN;	// Vector of current pix to light
varying vec3 vPixToEyeTBN;		// Vector of current pix to eye
varying vec3 vPosition;
varying vec3 vPositionNormalized;

uniform sampler2D texDiffuseMap;
uniform sampler2D texNormalHeightMap;
uniform sampler2D texDiffuse0;
uniform sampler2D texDiffuse1;
uniform sampler2D texDiffuse2;

uniform float parallax_factor;
uniform float detail_scale;
uniform float diffuse_scale;

uniform float water_height;
uniform vec3 fog_color;
uniform float time;

// Bounding Box of the terrain
uniform vec3 bbox_min;
uniform vec3 bbox_max;


vec4 NormalMapping(vec2 uv, vec3 vPixToEyeTBN, vec4 vPixToLightTBN, bool bParallax);
vec4 ReliefMapping(vec2 uv);
bool isUnderWater();

void main (void)
{
	vec4 vPixToLightTBNcurrent = vPixToLightTBN;
	
	gl_FragColor = NormalMapping(gl_TexCoord[0].st, vPixToEyeTBN, vPixToLightTBNcurrent, false);
}

bool isUnderWater()
{
	return (vPosition.y < water_height);
}

vec4 NormalMapping(vec2 uv, vec3 vPixToEyeTBN, vec4 vPixToLightTBN, bool bParallax)
{	
	vec3 lightVecTBN = normalize(vPixToLightTBN.xyz);
	vec3 viewVecTBN = normalize(vPixToEyeTBN);

	vec2 uv_detail = uv * detail_scale;
	vec2 uv_diffuse = uv * diffuse_scale;

	
	vec3 normalTBN = texture2D(texNormalHeightMap, uv_detail).rgb * 2.0 - 1.0;
	normalTBN = normalize(normalTBN);
	
	// LIGHTING :
	// Diffuse color
	vec4 tBase[3];
	tBase[0] = texture2D(texDiffuse0, uv_diffuse);
	tBase[1] = texture2D(texDiffuse1, uv_diffuse);	
	tBase[2] = texture2D(texDiffuse2, uv_diffuse);
	vec4 DiffuseMap = texture2D(texDiffuseMap, uv);
	
	vec4 cBase;
	// Calculate the color :
	if(vPosition.y < water_height)
		cBase = tBase[0];
	else {
		cBase = mix(mix(tBase[1], tBase[0], DiffuseMap.r), tBase[2], DiffuseMap.g);
	}


	float iDiffuse = max(dot(lightVecTBN.xyz, normalTBN), 0.0f);			// Diffuse intensity
	float iSpecular = 0.0f;
	
	if(isUnderWater())
		iSpecular = pow(clamp(dot(reflect(-lightVecTBN.xyz, normalTBN), viewVecTBN), 0.0, 1.0), gl_FrontMaterial.shininess )/2.0;
	
	
	
	vec4 cAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 cDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * iDiffuse;	
	vec4 cSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular * iSpecular;	

	return cAmbient * cBase + cDiffuse * cBase + cSpecular;
}