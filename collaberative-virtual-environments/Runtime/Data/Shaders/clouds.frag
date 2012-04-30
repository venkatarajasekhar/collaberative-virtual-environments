/*
varying vec3 vertex;
varying vec3 vertexMV;

uniform bool enable_sun;
uniform vec3 sun_vector;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;
uniform sampler2D Texture4;
uniform sampler2D Texture5;
uniform sampler2D Texture6;
uniform sampler2D Texture7;

uniform vec4 OctavesScales0;
uniform vec4 OctavesScales1;
uniform vec4 OctavesWeights0;
uniform vec4 OctavesWeights1;

uniform vec4 CloudsParams;		// x = cover, y = sharpness, z = pixoffset, w = ambient
uniform vec4 WindOffset;
uniform vec4 SunPos;
uniform vec4 SunColor;

#define fCloudsCover        CloudsParams.x
#define fCloudsSharpness    CloudsParams.y
#define fCloudsPixOffset    CloudsParams.z
#define fCloudsAmbient		CloudsParams.w

void main (void)
{
	if(enable_sun)
	{
		vec3 vert = normalize(vertex);
		vec3 sun = normalize(sun_vector);
		
	}
	else
	{



	}
}
*/

/*
varying vec3 vertex;
varying vec3 vertexMV;
varying vec2 texCoord;

uniform bool enable_sun;
uniform vec3 sun_vector;

uniform vec4 OctavesScales0;
uniform vec4 OctavesWeights0;

uniform vec4 CloudsParams;		// x = cover, y = sharpness, z = pixoffset, w = ambient

#define fCloudsCover        CloudsParams.x
#define fCloudsSharpness    CloudsParams.y
#define fCloudsPixOffset    CloudsParams.z
#define fCloudsAmbient		CloudsParams.w

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;

uniform sampler2D Texture7;

vec4 Clouds();

vec4 Clouds() 
{
	vec2 texcoord = texCoord;

    vec4 tex;
    vec4 cloud_cover;

    tex =  ((texture2D( Texture0, (texcoord * OctavesScales0.x )) - fCloudsPixOffset ) * OctavesWeights0.x );
    tex += ((texture2D( Texture1, (texcoord * OctavesScales0.y )) - fCloudsPixOffset ) * OctavesWeights0.y );
    tex += ((texture2D( Texture2, (texcoord * OctavesScales0.z )) - fCloudsPixOffset ) * OctavesWeights0.z );
    tex += ((texture2D( Texture3, (texcoord * OctavesScales0.w )) - fCloudsPixOffset ) * OctavesWeights0.w );

    cloud_cover = (texture2D( Texture7, texcoord) * 255.000);
    tex = (max( ((tex + 128.000) - cloud_cover), vec4( 0.000000 )) / 255.000);

    return vec4( tex.x , tex.x , tex.x , 1.00000);
}

void main()
{
    vec4 retVal;

    retVal = Clouds();

    gl_FragData[0] = vec4( retVal );
}
*/

uniform bool		enable_sun;
uniform vec3		sun_vector;

uniform vec4		OctavesScales0;
uniform vec4		OctavesWeights0;

uniform vec4		CloudsParams;		// x = cover, y = sharpness, z = pixoffset, w = ambient

#define fCloudsCover        CloudsParams.x
#define fCloudsSharpness    CloudsParams.y
#define fCloudsPixOffset    CloudsParams.z
#define fCloudsAmbient		CloudsParams.w

uniform sampler2D	tex0;

uniform vec3		wind_vec;
uniform float		tiling;
uniform vec2		offset;

varying vec3 position;

vec4 Clouds(vec4 color) 
{
	vec4 colorOut;

	vec2 Texcoord = gl_TexCoord[0].st * tiling;
	Texcoord.s += (offset.s);
	Texcoord.t += (offset.t);

	colorOut = texture2D( tex0, gl_TexCoord[0].st + Texcoord.st);

	// If raytracing enabled
	if(enable_sun)
	{
		float	Scattering = 0.0;
		vec3	EndTracePos = vec3(gl_TexCoord[0].st, -colorOut.r);					// Get cloud voxel
		vec3	TraceDir = EndTracePos - vec3(0.5, 0.5, sun_vector.y);				// Get trace direction
		TraceDir = normalize(TraceDir);												// Normalize it
		vec3	CurTracePos = vec3(0.5, 0.5, sun_vector.y) + TraceDir * 1.25;		// Scale trace position

		// Approximate light scattering integral from sun to current cloud voxel.
		TraceDir *= 16.0;
		for(int i=0; i<4; i++)					// OPTIMIZATION, less raymarching! Originally 64, 16 for speed atm.
		{
			CurTracePos += TraceDir;
			vec4	tex2 = texture2D(tex0, CurTracePos.xy);
			Scattering += (.1/255.0) * step(CurTracePos.z*2.0, tex2.r*2.0);			// Check if ray is inside clouds
		}

		float	Light = 1.0 / exp(Scattering * 0.4);
		colorOut = vec4(Light, Light, Light, colorOut.r);
	}

	// Make clouds darker
	if(sun_vector.y < -0.3)							// Evening
		colorOut *= (sun_vector.y * -1.0);
	else											// Night time
		colorOut *= 0.3;

	
	// Fade out clouds into the distance
	float fade = 1.0 - smoothstep(2600.0, 3200.0, length(position));
	colorOut.a *= fade;

	return colorOut;
}

void main(void)
{
	gl_FragColor = Clouds(gl_FragColor);
}
