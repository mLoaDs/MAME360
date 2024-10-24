// The name of this effect
string name : NAME = "Point";
float scaling : SCALING = 2.0;

matrix World				: WORLD;
matrix View				: VIEW;
matrix Projection			: PROJECTION;
matrix Worldview			: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection


string combineTechique : COMBINETECHNIQUE = "Point";


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


struct input
{
   float2 video_size;
   float2 texture_size;
   float2 output_size;
   float frame_count;
};

 
struct VS_IN
{                                                         
	float4 Position       	: POSITION;
	float2 UV        	: TEXCOORD0;                     
};

struct VS_OUT                                             
{                                                         
	float4 Position       	: POSITION;
	float2 UV        	: TEXCOORD0;                     
};

struct PS_IN
{                                                          
	float2 UV        	: TEXCOORD;                     
};

 
input vidParams : VIDPARAMS;

//
// Vertex Shader
//

VS_OUT VS( VS_IN In )
{
	VS_OUT Out;

	Out.Position = In.Position;
	Out.UV = In.UV;

	return Out;
}




float4 PS (in VS_OUT input) : COLOR
{
	return tex2D(SourceSampler, input.UV);
}


//
// Technique
//

technique Point
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS(); 
    }  
}
