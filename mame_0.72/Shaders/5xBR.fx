/*
   Hyllian's 5xBR v2.1 Shader
   
   Copyright (C) 2011 Hyllian/Jararaca - sergiogdb@gmail.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// The name of this effect
string name : NAME = "FiveTimesBR";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "FiveTimesBR";

texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

sampler	decal = sampler_state {
	Texture	  = (SourceTexture);
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
	AddressU  = Border;
	AddressV  = Border;
	AddressW  = Border;
};


const static float3 dtt = float3(65536,255,1);

float reduce(half3 color)
{
	return dot(color, dtt);
}


 
struct input
{
   float2 video_size;
   float2 texture_size;
   float2 output_size;
   float frame_count;
};

struct out_vertex {
	float4 position : POSITION;
	float4 color    : COLOR;
	float2 texCoord : TEXCOORD0;
	float4 t1 : TEXCOORD1;
};

 

input IN : VIDPARAMS;
 

 
//
// Vertex Shader
//

out_vertex  VS(			float4  position	: POSITION,
					float4  color		: COLOR,
					float2  texCoord    : TEXCOORD0
)
{
	 
	out_vertex OUT;

	OUT.position = position;
	OUT.color = color;

	float2 ps = float2(1.0/IN.texture_size.x, 1.0/IN.texture_size.y);
	float dx = ps.x;
	float dy = ps.y;

	OUT.texCoord = texCoord;
	OUT.t1.xy = float2(  0,-dy); // B
	OUT.t1.zw = float2(-dx,  0); // D

	return OUT;

 
}


float4 PS (in out_vertex VAR) : COLOR
{	
   
	float2 fp = frac(VAR.texCoord*IN.texture_size);

	float2 st = step(0.5,fp);

	float2 g1 = VAR.t1.xy*(st.x + st.y - 1.0) + VAR.t1.zw*(st.x - st.y    );
	float2 g2 = VAR.t1.xy*(st.y - st.x    ) + VAR.t1.zw*(st.x + st.y - 1.0);

	float AO  = 2.0*st.y - 1.0;
	float BO  = 2.0*st.x - 1.0;
	float CO  = st.x + st.y - 0.5;

	float AX  = 0.5*st.x + 1.5*st.y - 1.0;
	float BX  = 1.5*st.x - 0.5*st.y - 0.5;
	float CX  =     st.x + 0.5*st.y - 0.5;

	float AY  = -0.5*st.x + 1.5*st.y - 0.5;
	float BY  =  1.5*st.x + 0.5*st.y - 1.0;
	float CY  =  0.5*st.x +     st.y - 0.5;

	half3 A = tex2D(decal, VAR.texCoord +g1+g2).xyz;
	half3 B = tex2D(decal, VAR.texCoord +g1   ).xyz;
	half3 C = tex2D(decal, VAR.texCoord +g1-g2).xyz;
	half3 D = tex2D(decal, VAR.texCoord    +g2).xyz;
	half3 E = tex2D(decal, VAR.texCoord       ).xyz;
	half3 F = tex2D(decal, VAR.texCoord    -g2).xyz;
	half3 G = tex2D(decal, VAR.texCoord -g1+g2).xyz;
	half3 H = tex2D(decal, VAR.texCoord -g1   ).xyz;
	half3 I = tex2D(decal, VAR.texCoord -g1-g2).xyz;

	half3  C1 = tex2D(decal,VAR.texCoord +2.0*g1-g2   ).xyz;
	half3  G0 = tex2D(decal,VAR.texCoord -g1+2.0*g2   ).xyz;
	half3  C4 = tex2D(decal,VAR.texCoord +g1-2.0*g2   ).xyz;
	half3  I4 = tex2D(decal,VAR.texCoord -g1-2.0*g2   ).xyz;
	half3  G5 = tex2D(decal,VAR.texCoord -2.0*g1+g2   ).xyz;
	half3  I5 = tex2D(decal,VAR.texCoord -2.0*g1-g2   ).xyz;
	
	float a = reduce(A);
	float b = reduce(B);
	float c = reduce(C);
	float d = reduce(D);
	float e = reduce(E);
	float f = reduce(F);
	float g = reduce(G);
	float h = reduce(H);
	float i = reduce(I);

	float  c1 = reduce( C1);
	float  g0 = reduce( G0);
	float  c4 = reduce( C4);
	float  i4 = reduce( I4);
	float  g5 = reduce( G5);
	float  i5 = reduce( I5);
	
	float3 res = E;
	bool fx_1, fx_2, fx_3, condition_1, condition_1a, condition_1b, condition_2, condition_3, condition_4;
	bool condition_7, condition_8, condition_7a, condition_8a;

	fx_1        = (AO*fp.y+BO*fp.x > CO);
	fx_2        = (AX*fp.y+BX*fp.x > CX);
	fx_3        = (AY*fp.y+BY*fp.x > CY);
	condition_1a= ( e==c  &&  e==d  &&  h==g );
	condition_1b= ( e==g  &&  e==b  &&  f==c );
	condition_1 = (  e!=h  && (( h==f  && ( ( e!=i  && ( e!=b  ||  e!=d  ||  f!=b  &&  f!=c  ||  h!=d  &&  h!=g ))
	   || ( e==g  && ( i==h  ||  e==d  ||  h!=d )) || ( e==c  && ( i==h  ||  e==b  ||  f!=b ))))) );

	condition_2 = ( e!=h  &&  e!=f  && ( f!=i  &&  e==c  && ( h==i  &&  f!=b  ||  e!=i  &&  f==c4)));
	condition_3 = ( e!=h  &&  e!=f  && ( h!=i  &&  e==g  && ( f==i  &&  h!=d  ||  e!=i  &&  h==g5)));
	condition_4 = ( e!=h  &&  g==e  &&  e==c  &&  e!=i  &&  e!=f  );
	condition_7 = (  e!=f  && (( f==b  && ( ( e!=c  && ( e!=d  ||  e!=h  ||  b!=d  &&  b!=a  ||  f!=h  &&  f!=i ))
	   || ( e==i  && ( c==f  ||  e==h  ||  f!=h ))))
	) ); 
	condition_8 = (  e!=d  && (( d==h  && ( ( e!=g  && ( e!=f  ||  e!=b  ||  h!=f  &&  h!=i  ||  d!=b  &&  d!=a )) 
	   || ( e==i  && ( g==d  ||  e==f  ||  h!=f )) ))
	) ); 
	condition_7a  = ( e==a  &&  e==h  &&  f==i );
	condition_8a  = ( e==a  &&  e==f  &&  h==i );


	if (    (condition_1 && (fx_1 || (condition_1a && fx_2 || condition_1b && fx_3) ) ) ||
		(condition_8 && condition_8a && (BY*fp.y+AY*(1-fp.x) > CY)) || (fx_1 && condition_3)  )
        {
		res = H;
        }
	else if (  (condition_7 && condition_7a && (BX*(1-fp.y)+AX*fp.x > CX)) || (fx_1 && condition_2)  )
	{
		res = F;
	}
	else if (fx_1 && condition_4)
	{
		res = (F+H)*0.5;
	}

	return float4(res, 1.0);


 

}


//
// Technique
//

technique FiveTimesBR
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
