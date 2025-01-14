 
string name : NAME = "SuperEagleHD";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "SuperEagleHD";

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

struct out_vertex {
	float4 position : POSITION;
	//float4 color    : COLOR;
	float2 texCoord : TEXCOORD0;
	float4 t1 : TEXCOORD1;
	float4 t2 : TEXCOORD2;
	float4 t3 : TEXCOORD3;
	float4 t4 : TEXCOORD4;
	float4 t5 : TEXCOORD5;
	float4 t6 : TEXCOORD6;
	float4 t7 : TEXCOORD7;
	//float4 t8 : TEXCOORD8;
};


 


input IN : VIDPARAMS;
 
const static float3 dtt = float3(65536,255,1);

float reduce(half3 color)
{
	return dot(color, dtt);
}

 
//
// Vertex Shader
//

out_vertex VS(	float4 position	: POSITION,
	float4 color	: COLOR,
	float2 texCoord : TEXCOORD0)
{
 
	out_vertex OUT;

	OUT.position = position;
	//OUT.color = color;

	float2 ps = float2(1.0/768.0, 1.0/480.0);
	float dx = ps.x;
	float dy = ps.y;

	OUT.texCoord = texCoord;
	OUT.t1.xy = texCoord + half2(-dx,-dy);
	OUT.t1.zw = texCoord + half2(-dx,  0);
	OUT.t2.xy = texCoord + half2(+dx,-dy);
	OUT.t2.zw = texCoord + half2(+dx+dx,-dy);
	OUT.t3.xy = texCoord + half2(-dx,  0);
	OUT.t3.zw = texCoord + half2(+dx,  0);
	OUT.t4.xy = texCoord + half2(+dx+dx,  0);
	OUT.t4.zw = texCoord + half2(-dx,+dy);
	OUT.t5.xy = texCoord + half2(  0,+dy);
	OUT.t5.zw = texCoord + half2(+dx,+dy);
	OUT.t6.xy = texCoord + half2(+dx+dx,+dy);
	OUT.t6.zw = texCoord + half2(-dx,+dy+dy);
	OUT.t7.xy = texCoord + half2(  0,+dy+dy);
	OUT.t7.zw = texCoord + half2(+dx,+dy+dy);
	//OUT.t8.xy = texCoord + half2(+dx+dx,+dy+dy);

	return OUT;


 
}

 

/*  GET_RESULT function                            */
/*  Copyright (c) 1999-2001 by Derek Liauw Kie Fa  */
/*  License: GNU-GPL                               */
int GET_RESULT(float A, float B, float C, float D)
{
	int x = 0; int y = 0; int r = 0;
	if (A == C) x+=1; else if (B == C) y+=1;
	if (A == D) x+=1; else if (B == D) y+=1;
	if (x <= 1) r+=1; 
	if (y <= 1) r-=1;
	return r;
} 


float4 PS (in out_vertex VAR ) : COLOR
{	
   
	float2 fp = frac(VAR.texCoord*IN.texture_size);

	// Reading the texels

	half3 C0 = tex2D(decal,VAR.t1.xy).xyz; 
	half3 C1 = tex2D(decal,VAR.t1.zw).xyz;
	half3 C2 = tex2D(decal,VAR.t2.xy).xyz;
	half3 D3 = tex2D(decal,VAR.t2.zw).xyz;
	half3 C3 = tex2D(decal,VAR.t3.xy).xyz;
	half3 C4 = tex2D(decal,VAR.texCoord).xyz;
	half3 C5 = tex2D(decal,VAR.t3.zw).xyz;
	half3 D4 = tex2D(decal,VAR.t4.xy).xyz;
	half3 C6 = tex2D(decal,VAR.t4.zw).xyz;
	half3 C7 = tex2D(decal,VAR.t5.xy).xyz;
	half3 C8 = tex2D(decal,VAR.t5.zw).xyz;
	half3 D5 = tex2D(decal,VAR.t6.xy).xyz;
	half3 D0 = tex2D(decal,VAR.t6.zw).xyz;
	half3 D1 = tex2D(decal,VAR.t7.xy).xyz;
	half3 D2 = tex2D(decal,VAR.t7.zw).xyz;
	//half3 D6 = tex2D(decal,VAR.t8.xy).xyz;

	half3 p00,p10,p01,p11;

	// reducing half3 to float	
	float c0 = reduce(C0);float c1 = reduce(C1);
	float c2 = reduce(C2);float c3 = reduce(C3);
	float c4 = reduce(C4);float c5 = reduce(C5);
	float c6 = reduce(C6);float c7 = reduce(C7);
	float c8 = reduce(C8);float d0 = reduce(D0);
	float d1 = reduce(D1);float d2 = reduce(D2);
	float d3 = reduce(D3);float d4 = reduce(D4);
	float d5 = reduce(D5);
	//float d6 = reduce(D6);

	/*              SuperEagle code               */
	/*  Copied from the Dosbox source code        */
	/*  Copyright (C) 2002-2007  The DOSBox Team  */
	/*  License: GNU-GPL                          */
	/*  Adapted by guest(r) on 16.4.2007          */       
	if (c4 != c8) {
		if (c7 == c5) {
			p01 = p10 = C7;
			if ((c6 == c7) || (c5 == c2)) {
					p00 = 0.25*(3.0*C7+C4);
			} else {
					p00 = 0.5*(C4+C5);
			}

			if ((c5 == d4) || (c7 == d1)) {
					p11 = 0.25*(3.0*C7+C8);
			} else {
					p11 = 0.5*(C7+C8);
			}
		} else {
			p11 = 0.125*(6.0*C8+C7+C5);
			p00 = 0.125*(6.0*C4+C7+C5);

			p10 = 0.125*(6.0*C7+C4+C8);
			p01 = 0.125*(6.0*C5+C4+C8);
		}
	} else {
		if (c7 != c5) {
			p11 = p00 = C4;

			if ((c1 == c4) || (c8 == d5)) {
					p01 = 0.25*(3.0*C4+C5);
			} else {
					p01 = 0.5*(C4+C5);
			}

			if ((c8 == d2) || (c3 == c4)) {
					p10 = 0.25*(3.0*C4+C7);
			} else {
					p10 = 0.5*(C7+C8);
			}
		} else {
			int r = 0;
			r += GET_RESULT(c5,c4,c6,d1);
			r += GET_RESULT(c5,c4,c3,c1);
			r += GET_RESULT(c5,c4,d2,d5);
			r += GET_RESULT(c5,c4,c2,d4);

			if (r > 0) {
					p01 = p10 = C7;
					p00 = p11 = 0.5*(C4+C5);
			} else if (r < 0) {
					p11 = p00 = C4;
					p01 = p10 = 0.5*(C4+C5);
			} else {
					p11 = p00 = C4;
					p01 = p10 = C7;
			}
		}
	}



	// Distributing the four products

	p10 = (fp.x < 0.50) ? (fp.y < 0.50 ? p00 : p10) : (fp.y < 0.50 ? p01: p11);

	// OUTPUT
	return float4(p10, 1);


}


//
// Technique
//

technique SuperEagleHD
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
