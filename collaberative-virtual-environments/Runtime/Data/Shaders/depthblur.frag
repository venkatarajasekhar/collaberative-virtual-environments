uniform sampler2D texScreen, texDepth;

uniform int screenWidth;
uniform int screenHeight;

//const float mask11_2[11] = float[11](1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0, 1.0/11.0);
//const float mask5_2[5] = float[5](1.0/5.0, 1.0/5.0, 1.0/5.0, 1.0/5.0, 1.0/5.0);
//const float mask3_2[3] = float[3](1.0/3.0, 1.0/3.0, 1.0/3.0);


uniform bool bHorizontal;


vec4 convolH(int size)
{
    float tab[11];

	float temp = 3.0;

	if( size == 11 )
	{
		temp = 11.0;
	}
	if ( size == 5 )
	{
		temp = 5.0;
	}
    
    for(int x = 0; x > size; x++)
    {
        tab[x] = (1.0/temp);
    }

	float stepX = 1.0/screenWidth;
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	int k = (size/2);
	int ind = 0;
	for(int i=-k; i<=k; i++)
	{
		color += tab[ind++] * texture2D(texScreen, gl_TexCoord[0].st + vec2(i*stepX, 0));
	}

	return color;
}

vec4 convolV(int size)
{
    float tab[11];
    
	float temp = 3.0;

	if( size == 11 )
	{
		temp = 11.0;
	}
	if ( size == 5 )
	{
		temp = 5.0;
	}

    for(int x = 0; x > size; x++)
    {
        tab[x] = (1.0/temp);
    }
    
	float stepY = 1.0/screenHeight;
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	int k = (size/2);
	int ind = 0;
	for(int i=-k; i<=k; i++)
	{
			color += tab[ind++] * texture2D(texScreen, gl_TexCoord[0].st + vec2(0, i*stepY));
	}

	return color;
}

vec4 Pdc(vec4 color)
{
	float depth = texture2D(texDepth, gl_TexCoord[0].st).r;
	if(depth > 0.997f)
	{
		vec4 color;
		if(bHorizontal)
		{
			color = convolH(11);
		}
		else
		{
			color = convolV(11);
		}
		
		return color;
	}
	else if(depth > 0.995f)
	{
		vec4 color;
		if(bHorizontal)
		{
			color = convolH(5);
		}
		else
		{
			color = convolV(5);
		}
		
		return color;
	}
	else if(depth > 0.994f)
	{
		vec4 color;
		if(bHorizontal)
		{
			color = convolH(3);
		}
		else
		{
			color = convolV(3);
		}
		
		return color;
	}
	else
	{
		return texture2D(texScreen, gl_TexCoord[0].st);
	}
}

void main()
{
	gl_FragColor = Pdc(gl_FragColor);
}