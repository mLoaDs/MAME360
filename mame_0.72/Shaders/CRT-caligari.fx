// The name of this effect

/*
    Phosphor shader

    Copyright (C) 2010, 2011 caligari

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

string name : NAME = "Phosphor";
float scaling : SCALING = 2.0;
float2 ps : TEXELSIZE;

matrix World					: WORLD;
matrix View						: VIEW;
matrix Projection				: PROJECTION;
matrix Worldview				: WORLDVIEW;			// world * view
matrix ViewProjection			: VIEWPROJECTION;		// view * projection
matrix WorldViewProjection		: WORLDVIEWPROJECTION;		// world * view * projection

string combineTechique : COMBINETECHNIQUE = "Phosphor";

texture SourceTexture			: SOURCETEXTURE;
texture WorkingTexture			: WORKINGTEXTURE;
texture WorkingTexture1			: WORKINGTEXTURE1;

sampler	s_p  = sampler_state {
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
   float2 texCoord : TEXCOORD0;
   float2 onex : TEXCOORD1;
   //float2 oney : TEXCOORD2;
};

 


input IN : VIDPARAMS;
 

 
//
// Vertex Shader
//

tex_coords VS(			float4  position	: POSITION,
					float4  color		: COLOR,
					float2  texCoord    : TEXCOORD0,
					out float4 oPosition : POSITION,
					out float4 oColor    : COLOR					
)
{
 
   tex_coords coords;
	 
   oPosition = position;
   oColor = color;

   coords.texCoord = texCoord + float2(0.0, 0.0);
   
   coords.onex = float2(1.0 / IN.texture_size.x, 0.0);
   //coords.oney = float2(0.0, 1.0 / IN.texture_size.y);
            

   return coords;

 
}



// Comment the next line to disable interpolation in linear gamma (and gain speed).
    #define LINEAR_PROCESSING

    // Compensate for 16-235 level range as per Rec. 601.
    // #define REF_LEVELS

    // Simulate a CRT gamma of 2.4.
    #define inputGamma  2.4

    // Compensate for the standard sRGB gamma of 2.2.
    #define outputGamma 2.2

    #ifdef REF_LEVELS
	    #define LEVELS(c) max((c - 16.0 / 255.0) * 255.0 / (235.0 - 16.0), 0.0)
    #else
	    #define LEVELS(c) c
    #endif

    #ifdef LINEAR_PROCESSING
	    #define TEX2D(texture, c) pow(pow(LEVELS(tex2D((texture), (c))), float4(inputGamma,inputGamma,inputGamma,inputGamma)), float4(1.0 / outputGamma,1.0 / outputGamma,1.0 / outputGamma,1.0 / outputGamma))
    #else
	    #define TEX2D(texture, c) LEVELS(tex2D((texture), (c)))
    #endif




//	#define USE_ALL_NEIGHBOURS
  
	// 0.5 = same width as original pixel	1.0-1.2 seems nice
	#define SPOT_WIDTH	1.04
	
	// Shape of the spots	1.0 = circle, 4.0 = ellipse with width = 2*height  ************/
	#define X_SIZE_ADJUST	2.2
	
	/******************************** To increase bloom / luminosity play with this parameter ************/
	#define FACTOR_ADJUST   2.0

	#define TEXCOORDS	co.texCoord.xy

	#define SCALE	22.0

	// Constants
	const float4 luminosity_weights = float4( 0.2126, 0.7152, 0.0722, 0.0 );

    
	float factor( float lumi, float2 dxy)
	{	
		float dist = sqrt( dxy.x * dxy.x + dxy.y * dxy.y * X_SIZE_ADJUST  ) / SCALE;
		return (2.0 + lumi ) * (1.0 - smoothstep( 0.0, SPOT_WIDTH, dist ) ) / FACTOR_ADJUST ;
	}





float4 PS (in tex_coords co ) : COLOR
{	
   
	float2 coords_scaled = floor( TEXCOORDS * IN.texture_size * SCALE );
    	float2 coords_snes = floor( coords_scaled / SCALE );
    	float2 coords_texture = ( coords_snes + float2(0.5,0.5) ) / IN.texture_size;
    	    	
    	float2 ecart = coords_scaled - ( SCALE * coords_snes + float2( SCALE * 0.5 - 0.5, SCALE * 0.5 - 0.5 ) ) ;
    	
    	float4 color = TEX2D( s_p, coords_texture );
    	float luminosity = dot( color, luminosity_weights );
    	
    	color *= factor( luminosity, ecart );
    	    	
    	// RIGHT NEIGHBOUR
    	float4 pcol = TEX2D( s_p, coords_texture + co.onex);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( -SCALE , 0.0) );
    	
    	// LEFT NEIGHBOUR
	   	pcol = TEX2D( s_p, coords_texture - co.onex);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( SCALE , 0.0) );
    	
#ifdef USE_ALL_NEIGHBOURS
    	// TOP
	   	pcol = TEX2D( s_p, coords_texture + co.oney);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( 0.0, -SCALE) );

    	// TOP-LEFT
	   	pcol = TEX2D( s_p, coords_texture + co.oney - co.onex);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( SCALE, -SCALE) );
    	
    	// TOP-RIGHT
	   	pcol = TEX2D( s_p, coords_texture + co.oney + co.onex);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( -SCALE, -SCALE) );
    	
    	// BOTTOM
	   	pcol = TEX2D( s_p, coords_texture - co.oney);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( 0.0, SCALE) );

    	// BOTTOM-LEFT
	   	pcol = TEX2D( s_p, coords_texture - co.oney - co.onex);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( SCALE, SCALE) );
    	
    	// BOTTOM-RIGHT
	   	pcol = TEX2D( s_p, coords_texture - co.oney + co.onex);
    	luminosity = dot( pcol, luminosity_weights );
    	color += pcol * factor( luminosity, ecart + float2( -SCALE, SCALE) );
#endif

	return clamp( color, 0.0, 1.0 );


}


//
// Technique
//

technique Phosphor
{
    pass P0
    {
        // shaders		
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS(); 
    }  
}
