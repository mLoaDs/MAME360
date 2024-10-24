/*
   Author: Themaister
   License: Public domain
*/

// The name of this effect
string name : NAME = "Scanline";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "Scanline";

texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

sampler	s0 = sampler_state {
	Texture	  = (SourceTexture);
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
	AddressU  = Clamp;
	AddressV  = Clamp;
};


struct sine_coord
{
   float2 omega : TEXCOORD1;
};


 
struct input
{
   float2 video_size;
   float2 texture_size;
   float2 output_size;
   float frame_count;
};

input IN : VIDPARAMS;
 
const float base_brightness = 0.95;
const float2 sine_comp = float2(0.05, 0.15);
 
//
// Vertex Shader
//

sine_coord VS(		float4 position	: POSITION,
					out float4 oPosition : POSITION,
					float4 color	: COLOR,
					out float4 oColor : COLOR,
					float4 tex      : TEXCOORD0,
					out float2 oTex : TEXCOORD
)
{

   sine_coord coords;
	 
   oPosition = position;
   oColor = color;
   oTex = tex;

   float2 texsize = IN.texture_size;
   float2 delta = 0.5 / texsize;
   float dx = delta.x;
   float dy = delta.y;

   coords.omega = float2(3.1415 * IN.output_size.x * IN.texture_size.x / IN.video_size.x, 2.0 * 3.1415 * IN.texture_size.y);
    
  
   return coords;

 
}


float4  PS (in sine_coord co, float2 tex : TEXCOORD) : COLOR
{	
    
   float3 res = tex2D(s0, tex).xyz;
   float3 scanline = res * (base_brightness + dot(sine_comp * sin(tex * co.omega), float2(1.0,1.0)));
   return float4(scanline, 1.0);

 

}


//
// Technique
//

technique Scanline
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
