/*
   Author: Themaister
   License: Public domain
*/

 

// The name of this effect
string name : NAME = "StockAspect";
float scaling : SCALING = 2.0;

matrix World				: WORLD;
matrix View					: VIEW;
matrix Projection			: PROJECTION;
matrix Worldview			: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection


string combineTechique : COMBINETECHNIQUE = "StockAspect";


texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

 
sampler	s0 = sampler_state {
	Texture	  = (SourceTexture);
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE; 
};

struct input
{
   float2 video_size;
   float2 texture_size;
   float2 output_size;
   float frame_count;
};

 
  
input IN : VIDPARAMS;

//
// Vertex Shader
//


const float2 src0 = float2(0.6, 0.7);
const float2 src1 = float2(0.9, 0.9);
const float2 src2 = float2(-0.6, 0.3);
const float2 src3 = float2(0.1, 0.4);
const float2 src4 = float2(0.1, 0.4);
const float2 src5 = float2(0.5, 0.5);
const float2 src6 = float2(-1.0, 1.0);


float seed_random (float2 coords, float frame_cnt)
{
   frame_cnt = cos(frame_cnt);
   float2 constant1 = frame_cnt * float2(142.0/11.0, 1017.0/13.0);
   float constant2 = frame_cnt * 481344.0 / 11.0;
   return saturate(frac(sin(dot(coords, constant1)) * constant2) + 0.2);
}

void VS(    float4 position : POSITION,
   out float4 oPosition : POSITION,    
   float4 color : COLOR,
   out float4 oColor : COLOR,
   float2 tex : TEXCOORD,
   out float2 oTex : TEXCOORD)
{	 
    oPosition = position;
    oColor = color; 
	oTex = tex;		 
}




float4 PS (float2 tex : TEXCOORD) : COLOR
{  
   float2 texelSize = 1.0 / IN.texture_size;
   float rand = seed_random(tex, IN.frame_count);
   float4 color = tex2D(s0, tex.xy);
   color = (color - 16.0/255.0) * (255.0/219.0);
   return color * rand;
}


//
// Technique
//

technique StockAspect
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS(); 
    }  
}
