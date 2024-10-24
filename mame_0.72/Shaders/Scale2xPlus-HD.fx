 
string name : NAME = "ScaleTwoXPlusHD";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "ScaleTwoXPlusHD";

texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

sampler	decal  = sampler_state {
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

void VS(	float4 position	: POSITION,
	float4 color	: COLOR,
	float2 texCoord : TEXCOORD0,
	out float4 oPosition	: POSITION,
	out float4 oColor	: COLOR,
	out float2 otexCoord	: TEXCOORD	)
{
 
	oPosition = position;
	oColor = color;
	otexCoord = texCoord; 
 
}
 

float4 PS (float2 texCoord: TEXCOORD0) : COLOR
{	
   
	float2 ps = float2(0.999/512.0, 0.999/448.0);
	half2 dx = half2(ps.x, 0.0);
	half2 dy = half2(0.0, ps.y);

	float2 pixcoord = texCoord/ps;
	float2 fp	= frac(pixcoord);
	float2 d11	= texCoord-fp*ps;

	// Reading the texels

	half3 B = tex2D(decal,d11-dy).xyz;
	half3 D = tex2D(decal,d11-dx).xyz;
	half3 E = tex2D(decal,d11   ).xyz;
	half3 F = tex2D(decal,d11+dx).xyz;
	half3 H = tex2D(decal,d11+dy).xyz;
	
	half3 E0 = D == B && B != H && D != F ? D : E;
	half3 E1 = B == F && B != H && D != F ? F : E;
	half3 E2 = D == H && B != H && D != F ? D : E;
	half3 E3 = H == F && B != H && D != F ? F : E;
 
	return float4((E3*fp.x+E2*(1-fp.x))*fp.y+(E1*fp.x+E0*(1-fp.x))*(1-fp.y),1); ;


}


//
// Technique
//

technique ScaleTwoXPlusHD
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
