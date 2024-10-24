/*
   Author: Themaister
   License: Public domain
*/

 

// The name of this effect
string name : NAME = "Neon";
float scaling : SCALING = 2.0;

matrix World				: WORLD;
matrix View					: VIEW;
matrix Projection			: PROJECTION;
matrix Worldview			: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection


string combineTechique : COMBINETECHNIQUE = "Neon";


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
   float2 texsize = IN.texture_size;
   const float scale_factor = 1.0;
   float2 delta = 0.5 / (texsize * scale_factor);
   float dx = delta.x;
   float dy = delta.y;

   float3 c00 = tex2D(s0, tex + float2(-dx, -dy)).xyz;
   float3 c01 = tex2D(s0, tex + float2(-dx, 0)).xyz;
   float3 c02 = tex2D(s0, tex + float2(-dx, dy)).xyz;
   float3 c10 = tex2D(s0, tex + float2(0, -dy)).xyz;
   float3 c11 = tex2D(s0, tex + float2(0, 0)).xyz;
   float3 c12 = tex2D(s0, tex + float2(0, dy)).xyz;
   float3 c20 = tex2D(s0, tex + float2(dx, -dy)).xyz;
   float3 c21 = tex2D(s0, tex + float2(dx, 0)).xyz;
   float3 c22 = tex2D(s0, tex + float2(dx, dy)).xyz;
 
   float3 first = lerp(c00, c20, frac(scale_factor * tex.x * texsize.x + 0.5));
   float3 second = lerp(c02, c22, frac(scale_factor * tex.x * texsize.x + 0.5));

   float3 mid_horiz = lerp(c01, c21, frac(scale_factor * tex.x * texsize.x + 0.5));
   float3 mid_vert = lerp(c10, c12, frac(scale_factor * tex.y * texsize.y + 0.5));

   float3 res = lerp(first, second, frac(scale_factor * tex.y * texsize.y + 0.5));
   
   return float4(0.28 * (res + mid_horiz + mid_vert) + 4.7 * abs(res - lerp(mid_horiz, mid_vert, 0.5)), 1.0);
}


//
// Technique
//

technique Neon
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS(); 
    }  
}
