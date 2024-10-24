// The name of this effect
string name : NAME = "CRT";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "CRT";

texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

sampler	SourceSampler = sampler_state {
	Texture	  = (SourceTexture);
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
	AddressU  = Clamp;
	AddressV  = Clamp;
};


struct tex_coords
{
   float2 c01 : TEXCOORD0;
   float2 c11 : TEXCOORD1;
   float2 c21 : TEXCOORD2;
   float2 c31 : TEXCOORD3;
   float2 c02 : TEXCOORD4;
   float2 c12 : TEXCOORD5;
   float2 c22 : TEXCOORD6;
   float2 c32 : TEXCOORD7;
   float mod_factor : TEXCOORD8;
   float2 ratio_scale : TEXCOORD9;
};

struct input
{
   float2 video_size;
   float2 texture_size;
   float2 output_size;
   float frame_count;
};

input vidParams : VIDPARAMS;
 

#define PI 3.141592653589
#define gamma 2.7
 
//
// Vertex Shader
//

tex_coords VS(	float4 position : POSITION,
			out float4 oPosition : POSITION,			 
			float4 color : COLOR,
			out float4 oColor : COLOR,
			float2 tex : TEXCOORD
)
{
	 
	tex_coords coords;
		
	oPosition = position;
	oColor = color;

	float2 delta = 1.0 / vidParams.texture_size;
   	float dx = delta.x;
   	float dy = delta.y;
	 
	coords.c01 = tex + float2(-dx, 0.0);
	coords.c11 = tex + float2(0.0, 0.0);
	coords.c21 = tex + float2(dx, 0.0);
	coords.c31 = tex + float2(2.0 * dx, 0.0);
	coords.c02 = tex + float2(-dx, dy);
	coords.c12 = tex + float2(0.0, dy);
	coords.c22 = tex + float2(dx, dy);
	coords.c32 = tex + float2(2.0 * dx, dy);
	coords.mod_factor = tex.x * vidParams.output_size.x * vidParams.texture_size.x / vidParams.video_size.x;
	coords.ratio_scale = tex * vidParams.texture_size;

	return coords;
 
}


float4 PS (in tex_coords co) : COLOR
{	
   float2 uv_ratio = frac(co.ratio_scale);
   float3 col, col2;

   float4x3 texes0 = float4x3(tex2D(SourceSampler,(co.c01)).xyz, tex2D(SourceSampler,(co.c11)).xyz, tex2D(SourceSampler,(co.c21)).xyz, tex2D(SourceSampler,(co.c31)).xyz);
   float4x3 texes1 = float4x3(tex2D(SourceSampler,(co.c02)).xyz, tex2D(SourceSampler,(co.c12)).xyz, tex2D(SourceSampler,(co.c22)).xyz, tex2D(SourceSampler,(co.c32)).xyz);

   float4 coeffs = float4(1.0 + uv_ratio.x, uv_ratio.x, 1.0 - uv_ratio.x, 2.0 - uv_ratio.x) + 0.005;
   coeffs = sin(PI * coeffs) * sin(0.5 * PI * coeffs) / (coeffs * coeffs);
   coeffs = coeffs / dot(coeffs, float(1.0));

   float3 weights = float3(3.33 * uv_ratio.y,3.33 * uv_ratio.y,3.33 * uv_ratio.y);
   float3 weights2 = float3(uv_ratio.y * -3.33 + 3.33,uv_ratio.y * -3.33 + 3.33,uv_ratio.y * -3.33 + 3.33);

   col = saturate(mul(coeffs, texes0));
   col2 = saturate(mul(coeffs, texes1));

   float3 wid = 2.0 * pow(col, float3(4.0,4.0,4.0)) + 2.0;
   float3 wid2 = 2.0 * pow(col2, float3(4.0,4.0,4.0)) + 2.0;

   col = pow(col, float3(gamma,gamma,gamma));
   col2 = pow(col2, float3(gamma,gamma,gamma));

   float3 sqrt1 = rsqrt(0.5 * wid);
   float3 sqrt2 = rsqrt(0.5 * wid2);

   float3 pow_mul1 = weights * sqrt1;
   float3 pow_mul2 = weights2 * sqrt2;

   float3 div1 = 0.1320 * wid + 0.392;
   float3 div2 = 0.1320 * wid2 + 0.392;

   float3 pow1 = -pow(pow_mul1, wid);
   float3 pow2 = -pow(pow_mul2, wid2);

   weights = exp(pow1) / div1;
   weights2 = exp(pow2) / div2;

   float3 multi = col * weights + col2 * weights2;
   float3 mcol = lerp(float3(1.0, 0.7, 1.0), float3(0.7, 1.0, 0.7), floor(fmod(co.mod_factor, 2.0)));

   return float4(pow(mcol * multi, float3(0.454545,0.454545,0.454545)), 1.0);
 

}


//
// Technique
//

technique CRT
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
