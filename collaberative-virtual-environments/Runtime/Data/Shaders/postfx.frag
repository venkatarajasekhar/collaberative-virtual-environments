uniform sampler2D	texScreen, texBloom, texVignette, texWaterNoiseNM, texRain, texGui;

uniform sampler2D	texBruit, texBruit2;

uniform bool		enable_pdc, enable_underwater, enable_rain, enable_motionblur, enable_gui;

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

vec4 Bloom(vec4 color)
{
	return color + bloom_factor * texture2D(texBloom, gl_TexCoord[0].st);
}
/*
vec4 motionBlur(vec4 colorIn)
{
	vec4 tmp1,tmp2; 
	vec2 UV = gl_TexCoord[0].xy;
	//Retrieve depth of pixel  
	float z = texture2D(depthTexture, UV).z;  
	
	//Simplified equation of GluUnproject
	vec4 currentPos = vec4( 2.0* (gl_FragCoord.x/fWindowHeight)  - 1.0, 2.0* (gl_FragCoord.y/fWindowWidth) - 1.0, 2.0*z -1.0 , 1.0);

	//Back into the worldSpace 
	tmp1 =  currentPos  * inverseModelProjection  ;  
	
	//Homogenous value 
	vec4 posInWorldSpace = tmp1/tmp1.w;  
	
	//Using the world coordinate, we transform those into the previous frame
	tmp2 =  previousModelProjection *posInWorldSpace ;  
	vec4 previousPos = tmp2/tmp2.w;  
	
	//Compute the frame velocity using the difference 
	vec2 velocity = ((currentPos - previousPos)/10.0).xy;

	//Get the initial color at this pixel.  
	vec4 originalColor = texture2D(texture, UV);
	UV += velocity.xy;  
	for(int i = 1; i < 20.0; ++i)  
	{  
		//Sample the color buffer along the velocity vector.  
		vec4 currentColor = texture2D(texture, UV);  
		//Add the current color to our color sum.  
		originalColor += currentColor;  
		UV.x += velocity.x;
		UV.y += velocity.y;
	}  
	//Average all of the samples to get the final blur color.  
	return originalColor / 20.0;  
	
}
*/
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

// MICHAEL, SHADER GUI DRAWING IS HERE!
vec4 DrawGui(colorIn)
{
	vec4 colorOut;
	
	colorOut = colorIn +  texture2D(texGui, gl_TexCoord[0].st);	
	
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
		
	if(enable_gui)
		gl_FragColor = DrawGui(glFragColor);

	//if(enable_motionblur)
	//	gl_FragColor = motionBlur(gl_FragColor);

		
}
