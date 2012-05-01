uniform sampler2D	texRain;

uniform vec3		wind_vec;
uniform float		tile;
uniform vec3		offset;

uniform int			density;

vec4 Rain(vec4 colour, float depth)
{
	vec4 colourOut;
	
	vec2 Texcoord = gl_TexCoord[0].st * ((tile + 1) * (1.5*depth));
	Texcoord.s += (offset.s + (depth*0.1)) * (((density + 1) - depth) / density*2);
	Texcoord.t += (offset.t) * (((density + 1) - depth) / density*2);

	colourOut = colour + texture2D( texRain,  gl_TexCoord[0].st + Texcoord.st );

	float fade = 1.0 - smoothstep(0.0, (depth / density) * 0.5, length(gl_TexCoord[0].t));
	if( depth != 1 )
		colourOut.a *= fade;

	float fadeBottom = 1.0 - smoothstep( 0.0, 0.99, length(gl_TexCoord[0].t) );
	float fadeTop = 1.0 - smoothstep( 1.0, 0.01, length(gl_TexCoord[0].t) );

	colourOut.a *= fadeBottom;
	colourOut.a *= fadeTop;

	return colourOut;
}

void main(void)
{
	for(int x = 0; x < density; x++)
		gl_FragColor = Rain(gl_FragColor, x+1);
}
