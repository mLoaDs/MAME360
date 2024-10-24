/*

   Copyright (C) 2007 guest(r) - guest.r@gmail.com

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

/*

   The 4xSoft shader processes a gfx. surface and redraws it 4x finer.
   
   Note: set scaler to normal2x.

*/



// The name of this effect
string name : NAME = "FourTimesSoft";
float scaling : SCALING = 2.0;

matrix World				: WORLD;
matrix View					: VIEW;
matrix Projection			: PROJECTION;
matrix Worldview			: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection


string combineTechique : COMBINETECHNIQUE = "FourTimesSoft";


texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

 
sampler	s_p = sampler_state {
	Texture	  = (SourceTexture);
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
	AddressU  = Clamp;
	AddressV  = Clamp;
};


const static half3 dt = half3(1.0, 1.0, 1.0);

struct input
{
   float2 video_size;
   float2 texture_size;
   float2 output_size;
   float frame_count;
};


struct out_vertex {
	half4 position : POSITION;
	//half4 color    : COLOR;
	half2 texCoord : TEXCOORD0;
	half4 t1 : TEXCOORD1;
	half4 t2 : TEXCOORD2;
	half4 t3 : TEXCOORD3;
	half4 t4 : TEXCOORD4;
	half4 t5 : TEXCOORD5;
	half4 t6 : TEXCOORD6;
};
  
input IN : VIDPARAMS;

//
// Vertex Shader
//

out_vertex VS( 	half4 position	: POSITION,
			half4 color	: COLOR,
			half2 tex      : TEXCOORD0)
{
	half2 ps = half2(1.0/1344.0, 1.0/752.0);
	half dx = ps.x;
	half dy = ps.y;
	half sx = ps.x * 0.5;
	half sy = ps.y * 0.5;

	out_vertex OUT;

	OUT.position = position;
	//OUT.color = color;
	OUT.texCoord = tex;
	OUT.t1 = half4(tex,tex) + half4(-dx, -dy, dx, -dy); // outer diag. texels
	OUT.t2 = half4(tex,tex) + half4(dx, dy, -dx, dy); // inner diag. texels
	OUT.t3 = half4(tex,tex) + half4(-sx, -sy, sx, -sy);
	OUT.t4 = half4(tex,tex) + half4(sx, sy, -sx, sy); // inner hor/vert texels
	OUT.t5 = half4(tex,tex) + half4(-dx, 0, dx, 0);  // inner hor/vert texels
	OUT.t6 = half4(tex,tex) + half4(0, -dy, 0, dy);
 

	return OUT;
}




half4 PS (in out_vertex VAR) : COLOR
{
  
  half3 c11 = tex2D(s_p, VAR.texCoord).xyz;
  half3 c00 = tex2D(s_p, VAR.t1.xy).xyz;
  half3 c20 = tex2D(s_p, VAR.t1.zw).xyz;
  half3 c22 = tex2D(s_p, VAR.t2.xy).xyz;
  half3 c02 = tex2D(s_p, VAR.t2.zw).xyz;
  half3 s00 = tex2D(s_p, VAR.t3.xy).xyz;
  half3 s20 = tex2D(s_p, VAR.t3.zw).xyz;
  half3 s22 = tex2D(s_p, VAR.t4.xy).xyz;
  half3 s02 = tex2D(s_p, VAR.t4.zw).xyz;
  half3 c01 = tex2D(s_p, VAR.t5.xy).xyz;
  half3 c21 = tex2D(s_p, VAR.t5.zw).xyz;
  half3 c10 = tex2D(s_p, VAR.t6.xy).xyz;
  half3 c12 = tex2D(s_p, VAR.t6.zw).xyz;

  half d1=dot(abs(c00-c22),dt)+0.0001;
  half d2=dot(abs(c20-c02),dt)+0.0001;
  half hl=dot(abs(c01-c21),dt)+0.0001;
  half vl=dot(abs(c10-c12),dt)+0.0001;
  half m1=dot(abs(c00-c22),dt)+0.001;
  half m2=dot(abs(c02-c20),dt)+0.001;

  half3 t1=(hl*(c10+c12)+vl*(c01+c21)+(hl+vl)*c11)/(3.0*(hl+vl));
  half3 t2=(d1*(c20+c02)+d2*(c00+c22)+(d1+d2)*c11)/(3.0*(d1+d2));

  return half4(0.25*(t1+t2+(m2*(s00+s22)+m1*(s02+s20))/(m1+m2)),0);
}


//
// Technique
//

technique FourTimesSoft
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS(); 
    }  
}
