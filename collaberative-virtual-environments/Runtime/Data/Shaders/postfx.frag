uniform sampler2D	texScreen, texBloom, texVignette, texWaterNoiseNM, texRain;

uniform sampler2D	texBruit, texBruit2;

uniform bool		enable_pdc, enable_underwater, enable_rain;

uniform float		screenWidth;
uniform float		screnHeight;
uniform float		noise_tile;
uniform float		noise_factor, time;

uniform float		bloom_factor, blur_factor;

uniform bool		enable_bloom;
uniform bool		enable_vignette, enable_noise;

uniform vec3		wind_vec;
uniform float		rain_tile;
uniform vec3		rain_offset;


uniform float		randomCoeffNoise, randomCoeffFlash;

#define MAX_DEPTH	2

vec4 Bloom(vec4 color)
{
	return color + bloom_factor * texture2D(texBloom, gl_TexCoord[0].st);
}

vec4 Rain(vec4 color, float depth)
{
	vec4 colorOut;
	
	vec2 Texcoord = gl_TexCoord[0].st * ((rain_tile + 1) * (1.5*depth));
	Texcoord.s += (rain_offset.s + (depth*0.1)) * (((MAX_DEPTH + 1) - depth) / MAX_DEPTH*2);
	Texcoord.t += (rain_offset.t) * (((MAX_DEPTH + 1) - depth) / MAX_DEPTH*2);

	colorOut = color + texture2D( texRain,  gl_TexCoord[0].st + Texcoord.st );

	float fade = 1.0 - smoothstep(0.0, (depth / MAX_DEPTH) * 0.5, length(gl_TexCoord[0].t));

	if( depth != 1 )
		if( gl_TexCoord[0].t < fade )
			return color;

	return colorOut;
}

vec4 LevelOfGrey(vec4 colorIn)
{
	vec4 temp;
	temp.r *= 0.299;
	temp.g *= 0.587;
	temp.b *= 0.114;

	return temp;
}

vec4 NoiseEffect(vec4 colorIn)
{
	vec4 colorOut;
	
	vec4 colorNoise = texture2D(texBruit, gl_TexCoord[0].st + vec2(randomCoeffNoise, randomCoeffNoise));
	
	colorOut = mix(colorNoise, vec4(1,1,1,1), randomCoeffFlash) /3.0f + 2.0*colorIn/3.0f;
	colorOut.a = 0.3;

	return colorOut;
}

vec4 VignetteEffect(vec4 colorIn)
{
	
	vec4 ColorVignette = texture2D(texVignette, gl_TexCoord[0].st);
	
	vec4 colorOut = colorIn - (vec4(1,1,1,2)-ColorVignette);
	colorOut.r = clamp(colorOut.r,0.0,1.0);
	colorOut.g = clamp(colorOut.g,0.0,1.0);
	colorOut.b = clamp(colorOut.b,0.0,1.0);
	return colorOut;
}

vec4 UnderWater()
{
	vec4 colorOut;
	
	
	vec2 uvNormal0 = gl_TexCoord[0].st*noise_tile;
	uvNormal0.s += time*0.01;
	uvNormal0.t += time*0.01;
	vec2 uvNormal1 = gl_TexCoord[0].st*noise_tile;
	uvNormal1.s -= time*0.01;
	uvNormal1.t += time*0.01;
		
	vec3 normal0 = texture2D(texWaterNoiseNM, uvNormal0).rgb * 2.0 - 1.0;
	
	colorOut = texture2D(texScreen, gl_TexCoord[0].st + noise_factor*normal0.st);
	
	colorOut = clamp(colorOut, vec4(0.0, 0.0, 0.0, 0.0),  vec4(1.0, 1.0, 1.0, 1.0));
	
	return colorOut;
}


void main(void)
{
	
	
	if(enable_underwater)
	{
		gl_FragColor = UnderWater();
	}
	else
	{
		gl_FragColor = texture2D(texScreen, gl_TexCoord[0].st);
	}
	
	if(enable_bloom)
		gl_FragColor = Bloom(gl_FragColor);
	
	if(enable_noise)
	{
		gl_FragColor = LevelOfGrey(gl_FragColor);
		gl_FragColor = NoiseEffect(gl_FragColor);
	}
	
	if(enable_vignette)
		gl_FragColor = VignetteEffect(gl_FragColor);

	if(enable_rain)
	{
		for(int x = 1; x < MAX_DEPTH; x++)
			gl_FragColor = Rain(gl_FragColor, x);
	}
		
}
