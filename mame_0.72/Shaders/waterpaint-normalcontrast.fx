/*
   Author: Themaister
   License: Public domain
*/

string name : NAME = "WaterpaintNormal";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "WaterpaintNormal";

texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

sampler	s0  = sampler_state {
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

struct tex_coords
{
   float2 c00 : TEXCOORD0;
   float2 c01 : TEXCOORD1;
   float2 c02 : TEXCOORD2;
   float2 c10 : TEXCOORD3;
   float2 c11 : TEXCOORD4;
   float2 c12 : TEXCOORD5;
   float2 c20 : TEXCOORD6;
   float2 c21 : TEXCOORD7;
   float2 c22 : TEXCOORD8;
};

 


input IN : VIDPARAMS;
 

 
//
// Vertex Shader
//

tex_coords VS(		float4  position	: POSITION,
					float4  color		: COLOR,
					float2  tex    : TEXCOORD0,
					out float4 oPosition : POSITION,
					out float4 oColor    : COLOR					
)
{
 
   tex_coords coords;
	 
   oPosition = position;
   oColor = color;

   float2 texsize = IN.texture_size;
   const float scale_factor = 1.0;
   float2 delta = 0.499 / (texsize * scale_factor);
   float dx = delta.x;
   float dy = delta.y;

   coords.c00 = tex + float2(-dx, -dy);
   coords.c01 = tex + float2(-dx, 0);
   coords.c02 = tex + float2(-dx, dy);
   coords.c10 = tex + float2(0, -dy);
   coords.c11 = tex + float2(0, 0);
   coords.c12 = tex + float2(0, dy);
   coords.c20 = tex + float2(dx, -dy);
   coords.c21 = tex + float2(dx, 0);
   coords.c22 = tex + float2(dx, dy);

   

   return coords;

 
}

 

float4 compress(float4 in_color, float threshold, float ratio)
{
   float4 diff = in_color - float4(threshold,threshold,threshold,threshold);
   diff = clamp(diff, 0.0, 100.0);
   return in_color - (diff * (1.0 - 1.0/ratio));
}


float4 PS (in tex_coords co ) : COLOR
{	
   
   float2 texsize = IN.texture_size;
   const float scale_factor = 1.0;
   float2 tex = co.c11;

   float3 c00 = tex2D(s0, co.c00).xyz;
   float3 c01 = tex2D(s0, co.c01).xyz;
   float3 c02 = tex2D(s0, co.c02).xyz;
   float3 c10 = tex2D(s0, co.c10).xyz;
   float3 c11 = tex2D(s0, co.c11).xyz;
   float3 c12 = tex2D(s0, co.c12).xyz;
   float3 c20 = tex2D(s0, co.c20).xyz;
   float3 c21 = tex2D(s0, co.c21).xyz;
   float3 c22 = tex2D(s0, co.c22).xyz;

   float3 first = lerp(c00, c20, frac(scale_factor * tex.x * texsize.x + 0.5));
   float3 second = lerp(c02, c22, frac(scale_factor * tex.x * texsize.x + 0.5));

   float3 mid_horiz = lerp(c01, c21, frac(scale_factor * tex.x * texsize.x + 0.5));
   float3 mid_vert = lerp(c10, c12, frac(scale_factor * tex.y * texsize.y + 0.5));

   float3 res = lerp(first, second, frac(scale_factor * tex.y * texsize.y + 0.5));
   float4 final = float4(0.26 * (res + mid_horiz + mid_vert) + 3.5 * abs(res - lerp(mid_horiz, mid_vert, 0.5)), 1.0);

   return compress(final, 0.8, 5.0);
 


}


//
// Technique
//

technique WaterpaintNormal
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
