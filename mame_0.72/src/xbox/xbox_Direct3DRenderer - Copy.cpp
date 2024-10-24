/**
	* \file			xbox_Direct3DRenderer.cpp
	* \brief		Direct3D rendering subsystem
	*
	*	\note			Portions taken from XMAME
	*/

//= I N C L U D E S ====================================================
#include "xbox_Mame.h"

#include <stdarg.h>
#include <stdio.h>
#include <xgraphics.h>
#include <d3dx9effect.h>

#include "DebugLogger.h"
#include "GraphicsManager.h"
#include "xbox_Direct3DRenderer.h"

	// Font class from the XDK
#include "ATGFont.h"
#include "GFilterManager.h"
#include "ScalingEffect.h"
#include "hq2x_d3d.h"

extern "C" {
#include "osd_cpu.h"
#include "driver.h"
#include "vidhrdw/vector.h"     // For vector blitting
}


//= S T R U C T U R E S ===============================================
struct CUSTOMVERTEX
{
  D3DXVECTOR3   pos;  // The transformed position for the vertex
  FLOAT         tu, tv;   // The texture coordinates
};

 


// Projection matrices
D3DXMATRIX			m_matProj;
D3DXMATRIX			m_matWorld;
D3DXMATRIX			m_matView;

D3DXMATRIX			m_matPreProj;
D3DXMATRIX			m_matPreView;
D3DXMATRIX			m_matPreWorld;

// Pixel shader
char			pshader[255];
ScalingEffect*		psEffect;
LPDIRECT3DTEXTURE9		lpWorkTexture1;
LPDIRECT3DTEXTURE9		lpWorkTexture2;
LPDIRECT3DVOLUMETEXTURE9	lpHq2xLookupTexture;
 
LPDIRECT3DTEXTURE9		lpTexture;		// D3D texture
bool 			psEnabled;
bool			preProcess;

 
struct d3dvertex {
	float x, y, z; //screen coords
	float u, v;         //texture coords
};

static d3dvertex vertex[4];
 
 
static RECT Dest;
VOID* pPhysicalAddress = NULL;
 
 

//= D E F I N E S =====================================================


  // The color to clear the backbuffer to (set to non-black for debugging)
#define D3D_CLEAR_COLOR                 D3DCOLOR_XRGB(0,0,0)

//= G L O B A L = V A R S =============================================

	// The creation parameters passed to osd_create_display
static struct osd_create_params		g_createParams = {0};
static LPDIRECT3DDEVICE9		    g_pD3DDevice = NULL;
static ATG::Font				   *g_font = NULL;

static LPDIRECT3DTEXTURE9		    g_pTexture = NULL;
static LPDIRECT3DVERTEXBUFFER9      g_pD3DVertexBuffer = NULL;
static D3DVertexDeclaration*		g_pGradientVertexDecl = NULL;
static D3DVertexShader*				g_pGradientVertexShader = NULL;

 
D3DXHANDLE  g_technique;
D3DXHANDLE  g_tex;
LPD3DXEFFECT g_effect = NULL; //handle to D3DXEffect
 

	// Defines what portion of the texture is actually rendered to.
  // This rect is converted to TU/TV values in CreateRenderingQuad()
static RECT                       g_textureRenderingArea = {0,0,0,0};

		//! The locked region for us to render to
static D3DLOCKED_RECT             g_d3dLockedRect;

// Filter manager class
static CGFilterManager            g_FilterManger;

// This is for rendering frames into before they are scaled
static BYTE*                      g_pRenderBuffer = NULL;

// These will hold our original (pre filtered/scaled) width/height
static int                        g_OrigRenderWidth;
static int                        g_OrigRenderHeight;

extern "C" {
  //! Generic D3D Renderer options
RendererOptions_t                 g_rendererOptions;


//static int frame_count = 0;

extern UINT32	g_pal32Lookup[65536];
}

//= P R O T O T Y P E S ===============================================
static void Helper_RenderDirect16( void *destination, struct mame_bitmap *bitmap, const struct rectangle *bounds );
static void Helper_RenderDirect32( void *destination, struct mame_bitmap *bitmap, const struct rectangle *bounds );
static void Helper_RenderPalettized16( void *destination, struct mame_bitmap *bitmap, const struct rectangle *bounds );
static void Helper_RenderVectors( void *dest, struct mame_bitmap *bitmap, const struct rectangle *bnds, vector_pixel_t *vectorList );
static void Helper_AllocFilterBuffer();
static BOOL CreateTexture( void );
static BOOL CreateRenderingQuad( void );
extern "C" int fatalerror( const char *fmt, ... );

static int nTextureWidth = 0;
static int nTextureHeight = 0;
 
//-------------------------------------------------------------
//	InitializeD3DRenderer
//-------------------------------------------------------------
void InitializeD3DRenderer( CGraphicsManager &gman, ATG::Font *fnt )
{
	g_pD3DDevice = gman.GetD3DDevice();
	g_font = fnt;
	g_FilterManger.SetActiveFilter((EFilterType)g_rendererOptions.m_FilterType);

	preProcess = false;
}

extern "C" {


static inline int VidGetTextureSize(int size)
{
	int textureSize = 128;
	while (textureSize < size) {
		textureSize <<= 1;
	}
	return textureSize;
}

//-------------------------------------------------------------
//	SetScreenUsage
//-------------------------------------------------------------
void SetScreenUsage( FLOAT xPercentage, FLOAT yPercentage )
{
  g_rendererOptions.m_screenUsageX = xPercentage;
  g_rendererOptions.m_screenUsageY = yPercentage;
}

//-------------------------------------------------------------
//	GetScreenUsage
//-------------------------------------------------------------
void GetScreenUsage( FLOAT *xPercentage, FLOAT *yPercentage )
{
  if( xPercentage )
    *xPercentage = g_rendererOptions.m_screenUsageX;
  if( yPercentage )
    *yPercentage = g_rendererOptions.m_screenUsageY;
}

//-------------------------------------------------------------
//	SetScreenPosition
//-------------------------------------------------------------
void SetScreenPosition( FLOAT xOffset, FLOAT yOffset )
{
  g_rendererOptions.m_screenOffsetX = xOffset;
  g_rendererOptions.m_screenOffsetY = yOffset;
}

//-------------------------------------------------------------
//	GetScreenPosition
//-------------------------------------------------------------
void GetScreenPosition( FLOAT *xOffset, FLOAT *yOffset )
{
  if( xOffset )
    *xOffset = g_rendererOptions.m_screenOffsetX;
  if( yOffset )
    *yOffset = g_rendererOptions.m_screenOffsetY;
}
#pragma code_seg()

//-------------------------------------------------------------
//	D3DRendererCreateSession
//-------------------------------------------------------------
BOOL D3DRendererCreateSession( struct osd_create_params *params )
{
	PRINTMSG(( T_TRACE, "D3DRendererCreateSession" ));

    // Store the creation params
	memcpy( &g_createParams, params, sizeof(g_createParams) );

    // Fill out the orientation from the game driver
  g_createParams.orientation = (Machine->gamedrv->flags & ORIENTATION_MASK);

		// Flip the width and height
	if( g_createParams.orientation & ORIENTATION_SWAP_XY )
	{
		INT32 temp = g_createParams.height;
		g_createParams.height = g_createParams.width;
		g_createParams.width = temp;

		temp = g_createParams.aspect_x;
		g_createParams.aspect_x = g_createParams.aspect_y;
		g_createParams.aspect_y = temp;
	}
	
	// Get the active filters magnifiction level
	int iFilterMagLvl = g_FilterManger.GetActiveFilter().m_dwMagnificatonLvl;

	g_createParams.width  = g_createParams.width *= iFilterMagLvl;
	g_createParams.height = g_createParams.height *= iFilterMagLvl;

    // Assume that the game will render to the entire requested area
  g_textureRenderingArea.right = g_createParams.width;
  g_textureRenderingArea.bottom = g_createParams.height;

	// Figure out the bitmap depth and tell teh filter manager
	if( !(g_createParams.video_attributes & VIDEO_RGB_DIRECT) || g_createParams.depth >= 24 )
		g_FilterManger.SetBitmapDepth(32);
	else if( g_createParams.depth >= 15 )
		g_FilterManger.SetBitmapDepth(16);

		//-- Initialize the texture ---------------------------------------
  if( !CreateTexture() )
    return FALSE;



    //-- Initialize the rendering engine -------------------------------
 
    // Turn off culling
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
 
    // Turn off the zbuffer
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
 

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);


	if(preProcess) 
	{
    	    // Projection is (0,0,0) -> (1,1,1)
	    D3DXMatrixOrthoOffCenterLH(&m_matPreProj, -640,640,-320,320,0.0f ,1.0f);

	    // Align texels with pixels
	    D3DXMatrixTranslation(&m_matPreView, -0.5f/g_createParams.width, 0.5f/g_createParams.height, 0.0f);

	    // Identity for world
	    D3DXMatrixIdentity(&m_matPreWorld);



	}  
 
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 

	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);


	    // Create the quad that will be used to render the texture
  if( !CreateRenderingQuad() )
    return FALSE;


    //frame_count = 0;

	return TRUE;
}

//-------------------------------------------------------------
//	D3DRendererDestroySession
//-------------------------------------------------------------
void D3DRendererDestroySession( void )
{
 
	PRINTMSG(( T_TRACE, "D3DRendererDestroySession" ));

    g_pD3DDevice->SetStreamSource(0,NULL,0,NULL);	
    g_pD3DDevice->SetVertexShader( NULL );
    g_pD3DDevice->SetVertexDeclaration( NULL );
    g_pD3DDevice->SetPixelShader( NULL );
	g_pD3DDevice->SetTexture(0, NULL );
 

  if( g_pD3DVertexBuffer )
  {
    g_pD3DVertexBuffer->Release();
    g_pD3DVertexBuffer = NULL;
  }

	if( lpTexture )
	{
		lpTexture->Release();
		lpTexture = NULL;
	}

	if( lpWorkTexture1 )
	{
		lpWorkTexture1->Release();
		lpWorkTexture1 = NULL;
	
	}

	if( lpWorkTexture1 )
	{
		lpWorkTexture1->Release();
		lpWorkTexture1 = NULL;
	
	}

	if( lpHq2xLookupTexture )
	{
		lpHq2xLookupTexture->Release();
		lpHq2xLookupTexture = NULL;
	
	}
 
	if (g_pRenderBuffer != NULL)
	{
		delete [] g_pRenderBuffer;
		g_pRenderBuffer = NULL;
	}
 

	if (pPhysicalAddress)
	{
		XPhysicalFree(pPhysicalAddress);
		pPhysicalAddress = NULL;
	}

	if (psEffect)
	{		
		psEffect->KillThis();
		delete psEffect;
		psEffect = NULL;
	}
}

//-------------------------------------------------------------
//	D3DRendererSetOutputRect
//-------------------------------------------------------------
void D3DRendererSetOutputRect( INT32 left, INT32 top, INT32 right, INT32 bottom )
{
	// 1) Tear down the preexisting texture
	D3DRendererDestroySession();  

	// Get the active filters magnifiction level
	int iFilterMagLvl = g_FilterManger.GetActiveFilter().m_dwMagnificatonLvl;

    // 2) Set up the new creation params
	if( g_createParams.orientation & ORIENTATION_SWAP_XY )
	{
		// Need to flip the width and height
		g_createParams.width = bottom * iFilterMagLvl;
		g_createParams.height = right * iFilterMagLvl;

		g_textureRenderingArea.left = top * iFilterMagLvl;
		g_textureRenderingArea.top = left * iFilterMagLvl;
		g_textureRenderingArea.right = (bottom + 1) * iFilterMagLvl;
		g_textureRenderingArea.bottom = (right + 1) * iFilterMagLvl;
	}
	else
	{
		g_createParams.width = right;
		g_createParams.height = bottom;

		g_textureRenderingArea.left = left * iFilterMagLvl;
		g_textureRenderingArea.top = top * iFilterMagLvl;
		g_textureRenderingArea.right = (right + 1) * iFilterMagLvl;
		g_textureRenderingArea.bottom = (bottom + 1) * iFilterMagLvl;
	}

    // 3) Create a new texture
	CreateTexture();
 
    // 4) Create a new quad
	CreateRenderingQuad();  

	// Allocate our buffers
	Helper_AllocFilterBuffer();

}


//-------------------------------------------------------------
//	D3DRendererRender
//-------------------------------------------------------------
BOOL D3DRendererRender(	struct mame_bitmap *bitmap,
												const struct rectangle *bounds,
												void *vector_dirty_pixels )
{

	//frame_count++;

	lpTexture->LockRect(	0, &g_d3dLockedRect, NULL, D3DLOCK_NOOVERWRITE);
   
  
		// Clear the screen
	g_pD3DDevice->Clear(	0L,																// Count
												NULL,															// Rects to clear
												D3DCLEAR_TARGET,	// Flags
												D3D_CLEAR_COLOR,							// Color
												1.0f,															// Z
												0L );															// Stencil

	if( !bitmap )
	{
      // Render the debug console
    RenderDebugConsole( g_pD3DDevice );

			// Present the cleared screen
		g_pD3DDevice->Present( NULL, NULL, NULL, NULL );

		return TRUE;
	}

  if( vector_dirty_pixels )
  {
    Helper_RenderVectors( g_d3dLockedRect.pBits, bitmap, bounds, (vector_pixel_t*)vector_dirty_pixels );
  }
  else
  {
		  // Blit the bitmap to the texture
	  if( bitmap->depth == 15 || bitmap->depth == 16 )
	  {
		  if( g_createParams.video_attributes & VIDEO_RGB_DIRECT )
		  {
				  // Destination buffer is in 15 bit X1R5G5B5
        Helper_RenderDirect16( g_d3dLockedRect.pBits, bitmap, bounds );
		  }
		  else
		  {
				  // Have to translate the colors through the palette lookup table
			  Helper_RenderPalettized16( g_d3dLockedRect.pBits, bitmap, bounds );
		  }
	  }
    else if( bitmap->depth == 32 )
	  {
		  if( !(g_createParams.video_attributes & VIDEO_RGB_DIRECT) )
        fatalerror( "Palettized 32 bit mode is not supported.\nPlease report this game immediately!" );
      else
		    Helper_RenderDirect32( g_d3dLockedRect.pBits, bitmap, bounds );
	  }	
    else
      fatalerror( "Attempt to render with unknown depth %lu!\nPlease report this game immediately!", bitmap->depth );
  }

	lpTexture->UnlockRect(0);
 
    HRESULT hr;
    UINT uPasses, psActive = 1;
 

	psEffect->SetVideoInputs(nTextureWidth,nTextureHeight,1280,720/*,frame_count*/);
	psEffect->SetMatrices(m_matProj,m_matView,m_matWorld);
	psEffect->SetTextures(lpTexture, lpWorkTexture1, lpWorkTexture2, lpHq2xLookupTexture);
 

	uPasses = 0;	 

	psEffect->Begin(ScalingEffect::Combine, &uPasses);	 
	hr=psEffect->BeginPass(0);	    
	// Render the vertex buffer contents
	g_pD3DDevice->DrawPrimitive(D3DPT_QUADLIST, 0, 1 );
	psEffect->EndPass();	
	psEffect->End();
 
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
 
 
	return TRUE;
}

} // End Extern "C"



//-------------------------------------------------------------
//  Helper_AllocFilterBuffer
//-------------------------------------------------------------
static void Helper_AllocFilterBuffer( void )
{
	if( g_pRenderBuffer )
	{
		delete[] g_pRenderBuffer;
		g_pRenderBuffer = NULL;
	}

	  // Get the active filter info
	SFilterInfo oActiveFilter = g_FilterManger.GetActiveFilter();

	  // Store our original width and height
	g_OrigRenderWidth  = g_createParams.width/oActiveFilter.m_dwMagnificatonLvl;
	g_OrigRenderHeight = g_createParams.height/oActiveFilter.m_dwMagnificatonLvl;

	  // Don't do anything if no filter is selected
	if( oActiveFilter.m_FilterType == eftNone )
		return;

	  // Figure out our game bit-depth 2 or 4 bytes (assume 2 check if it is 4)
	int iDepth = 2; 

	  // Use 32 bit color for palettized or 32 bit sessions
	if( !(g_createParams.video_attributes & VIDEO_RGB_DIRECT) || g_createParams.depth >= 24 )
		iDepth = 4;

	  // Calculate the original (pre scaled) bitmap size
	int iOrigBitampBytes = (g_OrigRenderWidth*g_OrigRenderHeight*iDepth);

	  // Allocate the render buffer large enough to hold a original sized game frame
	g_pRenderBuffer = new BYTE[iOrigBitampBytes];
}

//-------------------------------------------------------------
//	Helper_RenderDirect16
//-------------------------------------------------------------
static void Helper_RenderDirect16( void *dest, struct mame_bitmap *bitmap, const struct rectangle *bnds )
{
	struct rectangle bounds = *bnds;
	++bounds.max_x;
	++bounds.max_y;

	UINT16 *destBuffer;
	UINT16 *sourceBuffer = (UINT16*)bitmap->base;

	  // If we a filtering render into a temp buffer then filter that buffer
	  // into the actual framebuffer
	if (g_FilterManger.GetActiveFilter().m_FilterType != eftNone)
		destBuffer = (UINT16*)g_pRenderBuffer;
	else
		destBuffer = (UINT16*)dest;

	if( g_createParams.orientation & ORIENTATION_SWAP_XY )
  {
	  sourceBuffer += (bounds.min_y * bitmap->rowpixels) + bounds.min_x;

      // SwapXY 
    destBuffer += bounds.min_y;  // The bounds.min_y value gives us our starting X coord
    destBuffer += (bounds.min_x * g_OrigRenderWidth); // The bounds.min_x value gives us our starting Y coord

      // Render, treating sourceBuffer as normal (x and y not swapped)
    for( UINT32 y = bounds.min_y; y < bounds.max_y; ++y )
    {
      UINT16	*offset = destBuffer;
      UINT16  *sourceOffset = sourceBuffer;

			for( UINT32 x = bounds.min_x; x < bounds.max_x; ++x )
      {
        *offset = *(sourceOffset++);
        offset += g_OrigRenderWidth;   // Increment the output Y value
      }

      sourceBuffer += bitmap->rowpixels;
      ++destBuffer;          // Come left ("down") one row
    }
  }
  else
	{ 
	  sourceBuffer += (bounds.min_y * bitmap->rowpixels) + bounds.min_x;
    destBuffer += ((bounds.min_y * g_OrigRenderWidth) + bounds.min_x);

    UINT32 scanLen = (bounds.max_x - bounds.min_x) << 1;

		for( UINT32 y = bounds.min_y; y < bounds.max_y; ++y )
		{
      memcpy( destBuffer, sourceBuffer, scanLen );
			destBuffer += g_OrigRenderWidth;
			sourceBuffer += bitmap->rowpixels;
		}
  }

	 
}

//-------------------------------------------------------------
//	Helper_RenderDirect32
//-------------------------------------------------------------
static void Helper_RenderDirect32( void *dest, struct mame_bitmap *bitmap, const struct rectangle *bnds )
{
	struct rectangle bounds = *bnds;
	++bounds.max_x;
	++bounds.max_y;

		// 32 bit direct
  if( !(g_createParams.video_attributes & VIDEO_RGB_DIRECT) )
  {
		PRINTMSG(( T_ERROR, "32 bit palettized mode not supported!" ));
    return;
  }
	
	UINT32 *destBuffer;
	UINT32 *sourceBuffer = (UINT32*)bitmap->base;

	  // If we a filtering render into a temp buffer then filter that buffer
	  // into the actual framebuffer
	if (g_FilterManger.GetActiveFilter().m_FilterType != eftNone)
		destBuffer = (UINT32*)g_pRenderBuffer;
	else
		destBuffer = (UINT32*)dest;

  	// Destination buffer is in 32 bit X8R8G8B8
	if( g_createParams.orientation & ORIENTATION_SWAP_XY )
  {
	  sourceBuffer += (bounds.min_y * bitmap->rowpixels) + bounds.min_x;

      // SwapXY 
    destBuffer += bounds.min_y;  // The bounds.min_y value gives us our starting X coord
    destBuffer += (bounds.min_x * g_OrigRenderWidth); // The bounds.min_x value gives us our starting Y coord

      // Render, treating sourceBuffer as normal (x and y not swapped)
    for( UINT32 y = bounds.min_y; y < bounds.max_y; ++y )
    {
      UINT32	*offset = destBuffer;
      UINT32  *sourceOffset = sourceBuffer;

			for( UINT32 x = bounds.min_x; x < bounds.max_x; ++x )
      {
        *offset = *(sourceOffset++);
        offset += g_OrigRenderWidth;   // Increment the output Y value
      }

      sourceBuffer += bitmap->rowpixels;
      ++destBuffer;          // Move right ("down") one row
    }
  }
  else
	{ 
	  sourceBuffer += (bounds.min_y * bitmap->rowpixels) + bounds.min_x;

    destBuffer += ((bounds.min_y * g_OrigRenderWidth) + bounds.min_x);
    UINT32 scanLen = (bounds.max_x - bounds.min_x) << 2;

		for( UINT32 y = bounds.min_y; y < bounds.max_y; ++y )
		{
      memcpy( destBuffer, sourceBuffer, scanLen );
			destBuffer += g_OrigRenderWidth;
			sourceBuffer += bitmap->rowpixels;
		}
  }

	 
}



//-------------------------------------------------------------
//	Helper_RenderPalettized16
//-------------------------------------------------------------
static void Helper_RenderPalettized16( void *dest, struct mame_bitmap *bitmap, const struct rectangle *bnds )
{
	struct rectangle bounds = *bnds;
	++bounds.max_x;
	++bounds.max_y;

	UINT32 *destBuffer;
	UINT16 *sourceBuffer = (UINT16*)bitmap->base;

	  // If we a filtering render into a temp buffer then filter that buffer
	  // into the actual framebuffer
	if (g_FilterManger.GetActiveFilter().m_FilterType != eftNone)
		destBuffer = (UINT32*)g_pRenderBuffer;
	else
		destBuffer = (UINT32*)dest;

		// bitmap format is 16 bit indices into the palette
		// Destination buffer is in 32 bit X8R8G8B8
	if( g_createParams.orientation & ORIENTATION_SWAP_XY )
	{ 
    sourceBuffer += (bounds.min_y * bitmap->rowpixels) + bounds.min_x;

      // SwapXY
    destBuffer += bounds.min_y;  // The bounds.min_y value gives us our starting X coord
    destBuffer += (bounds.min_x * g_OrigRenderWidth); // The bounds.min_x value gives us our starting Y coord

      // Render, treating sourceBuffer as normal (x and y not swapped)
    for( UINT32 y = bounds.min_y; y < bounds.max_y; ++y )
    {
      UINT32	*offset = destBuffer;
      UINT16  *sourceOffset = sourceBuffer;

			for( UINT32 x = bounds.min_x; x < bounds.max_x; ++x )
      {
          // Offset is in RGBX format	
        *offset = g_pal32Lookup[ *(sourceOffset++) ];

          // Skip to the next row
        offset += g_OrigRenderWidth;   // Increment the output Y value
      }

      sourceBuffer += bitmap->rowpixels;
      ++destBuffer;          // Come left ("down") one row
    }
	}
	else
	{
    sourceBuffer += (bounds.min_y * bitmap->rowpixels) + bounds.min_x;
    destBuffer += (bounds.min_y * g_OrigRenderWidth + bounds.min_x);

		for( UINT32 y = bounds.min_y; y < bounds.max_y; ++y )
		{
			UINT32	*offset = destBuffer;
			UINT16  *sourceOffset = sourceBuffer;

			for( UINT32 x = bounds.min_x; x < bounds.max_x; ++x )
			{
          // Offset is in RGBX format	
        *(offset++) = g_pal32Lookup[ *(sourceOffset++) ];
			}

			destBuffer += g_OrigRenderWidth;
      sourceBuffer += bitmap->rowpixels;
		}
	}

	 
}
    


//-------------------------------------------------------------
//	Helper_RenderVectors
//-------------------------------------------------------------
static void Helper_RenderVectors( void *dest, struct mame_bitmap *bitmap, const struct rectangle *bnds, vector_pixel_t *vectorList )
{
  if( !bitmap || !bitmap->base || !dest )
    return;

  // [EBA] - For some reason the bounds stuff doesn't seem to be needed at all (at least in 0.72 and later)
  //         it's already taken care of in the vectorList

  if( bitmap->depth == 32 )
  {
    UINT32 *destBuf = (UINT32*)dest;
    UINT32 *sourceBuf = (UINT32*)bitmap->base;

	  if( g_createParams.orientation & ORIENTATION_SWAP_XY )
    {
	    while( *vectorList != VECTOR_PIXEL_END )
	    {
		    vector_pixel_t coords = *(vectorList++);
		    INT32 x = VECTOR_PIXEL_X( coords );
		    INT32 y = VECTOR_PIXEL_Y( coords );
        destBuf[ y + x * g_createParams.width ] = sourceBuf[ x + y * bitmap->rowpixels ];
	    }
    }
    else
    {
	    while( *vectorList != VECTOR_PIXEL_END )
	    {
		    vector_pixel_t coords = *(vectorList++);
		    INT32 x = VECTOR_PIXEL_X( coords );
		    INT32 y = VECTOR_PIXEL_Y( coords );
        destBuf[ x + y * g_createParams.width ] = sourceBuf[ x + y * bitmap->rowpixels ];
	    }
    }
  }
  else
  {
    UINT16 *destBuf = (UINT16*)dest;
    UINT16 *sourceBuf = (UINT16*)bitmap->base;

	  if( g_createParams.orientation & ORIENTATION_SWAP_XY )
    {
	    while( *vectorList != VECTOR_PIXEL_END )
	    {
		    vector_pixel_t coords = *(vectorList++);
		    INT32 x = VECTOR_PIXEL_X( coords );
		    INT32 y = VECTOR_PIXEL_Y( coords );
        UINT32 color = g_pal32Lookup[ sourceBuf[ x + y * bitmap->rowpixels ] ];
        destBuf[ y + x * g_createParams.width ] = color;
	    }
    }
    else
    {
	    while( *vectorList != VECTOR_PIXEL_END )
	    {
		    vector_pixel_t coords = *(vectorList++);
		    INT32 x = VECTOR_PIXEL_X( coords );
		    INT32 y = VECTOR_PIXEL_Y( coords );
        UINT32 color = g_pal32Lookup[ sourceBuf[ x + y * bitmap->rowpixels ] ];
        destBuf[ x + y * g_createParams.width ] = color;
	    }
    }
  }
}






//-------------------------------------------------------------
//  CreateTexture
//-------------------------------------------------------------
static BOOL CreateTexture( void )
{
	if( lpTexture )
	{
		lpTexture->Release();
		lpTexture = NULL;
	}

	if( lpWorkTexture1 )
	{
		lpWorkTexture1->Release();
		lpWorkTexture1 = NULL;
	
	}

	if( lpWorkTexture1 )
	{
		lpWorkTexture1->Release();
		lpWorkTexture1 = NULL;
	
	}

	if( lpHq2xLookupTexture )
	{
		lpHq2xLookupTexture->Release();
		lpHq2xLookupTexture = NULL;
	
	}

		// Create the texture	
	D3DFORMAT textureFormat;
	HRESULT hr;

		// Use 32 bit color for palettized or 32 bit sessions
	if( !(g_createParams.video_attributes & VIDEO_RGB_DIRECT) || g_createParams.depth >= 24 )
		textureFormat = D3DFMT_LIN_X8R8G8B8;
	else if( g_createParams.depth >= 15 )
		textureFormat = D3DFMT_LIN_X1R5G5B5;

	nTextureWidth =  VidGetTextureSize(g_createParams.width);
	nTextureHeight =  VidGetTextureSize(g_createParams.height);
 

  if( (D3DXCreateTexture( g_pD3DDevice,
                          nTextureWidth,
                          nTextureHeight,
													1,									// Mip levels
                          0,                  // Usage
													textureFormat,      // Format
													0,		              // Pool (unused)
                          &lpTexture )) != S_OK )
  {
    MEMORYSTATUS memStatus;
    GlobalMemoryStatus(  &memStatus );

    PRINTMSG(( T_ERROR, "Failed to create texture" ));
    PRINTMSG(( T_INFO, "Memory status" ));
    PRINTMSG(( T_INFO, "Physical:" ));
    PRINTMSG(( T_INFO, "         Avail: %lu", memStatus.dwAvailPhys ));
    PRINTMSG(( T_INFO, "         Total: %lu", memStatus.dwTotalPhys ));
    PRINTMSG(( T_INFO, "Page File:" ));
    PRINTMSG(( T_INFO, "         Avail: %lu", memStatus.dwAvailPageFile ));
    PRINTMSG(( T_INFO, "         Total: %lu", memStatus.dwTotalPageFile ));
    PRINTMSG(( T_INFO, "Virtual:" ));
    PRINTMSG(( T_INFO, "         Avail: %lu", memStatus.dwAvailVirtual ));
    PRINTMSG(( T_INFO, "         Total: %lu", memStatus.dwTotalVirtual ));

		return FALSE;
	}

		// Grab the surface description
	D3DSURFACE_DESC desc;
	lpTexture->GetLevelDesc( 0, &desc );

    // Since the texture may have taken width and height values other than the
    // ones we specified, make sure they're correct now. These values are used
    // as the stride for the Render* functions
	g_createParams.width = desc.Width;
	g_createParams.height = desc.Height;
 
 
	// Working textures for pixel shader
	if(FAILED(hr=g_pD3DDevice->CreateTexture(nTextureWidth, nTextureHeight, 1, 0,
			    textureFormat, D3DPOOL_DEFAULT, &lpWorkTexture1, NULL))) {
 
	    return E_FAIL;
	}

	if(FAILED(hr=g_pD3DDevice->CreateTexture(nTextureWidth, nTextureWidth, 1, 0,
			    textureFormat, D3DPOOL_DEFAULT, &lpWorkTexture2, NULL))) {
 
	    return E_FAIL;
	}

	if(FAILED(hr=g_pD3DDevice->CreateVolumeTexture(256, 16, 256, 1, 0, textureFormat,
			    0, &lpHq2xLookupTexture, NULL))) {
 
	    return E_FAIL;
	}

	// build lookup table
	D3DLOCKED_BOX lockedBox;

	if(FAILED(hr = lpHq2xLookupTexture->LockBox(0, &lockedBox, NULL, 0))) {
 
	    return E_FAIL;
	}

	BuildHq2xLookupTexture(1280, 720, nTextureWidth, nTextureHeight, (Bit8u *)lockedBox.pBits);

 

	if(FAILED(hr = lpHq2xLookupTexture->UnlockBox(0))) {
 
	    return E_FAIL;
	}

	// Clear the textures first otherwise we may have artifacts

	lpTexture->LockRect(	0, &g_d3dLockedRect, NULL, 0 );
	memset( g_d3dLockedRect.pBits, 0, desc.Width * desc.Height * sizeof(DWORD) );
	lpTexture->UnlockRect(0);

	lpWorkTexture1->LockRect(	0, &g_d3dLockedRect, NULL, 0 );
	memset( g_d3dLockedRect.pBits, 0, desc.Width * desc.Height * sizeof(DWORD) );
	lpWorkTexture1->UnlockRect(0);

	lpWorkTexture2->LockRect(	0, &g_d3dLockedRect, NULL, 0 );
	memset( g_d3dLockedRect.pBits, 0, desc.Width * desc.Height * sizeof(DWORD) );
	lpWorkTexture2->UnlockRect(0);
	

	return TRUE;
}

 
static inline int dx9SetVertex(
	unsigned int px, unsigned int py, unsigned int pw, unsigned int ph,
    unsigned int tw, unsigned int th,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h
    )
{

#if 0
	int nXOffset = 0;
	int nYOffset = 0;
	int nXScale = 0;
	int nYScale = 0;
 

	// configure triangles
	// -0.5f is necessary in order to match texture alignment to display pixels
	float diff = -0.5f;

	void *pLockedVertexBuffer;

 switch( g_rendererOptions.m_screenRotation )
    {
    case SR_180:
			vertex[2].x = vertex[3].x = (double)(y    ) + diff + nXScale;
			vertex[0].x = vertex[1].x = (double)(y + h) + diff;
			vertex[1].y = vertex[3].y = (double)(x + w) + diff + nYScale;
			vertex[0].y = vertex[2].y = (double)(x    ) + diff;
      break;

    case SR_270:
			vertex[0].x = vertex[1].x = (double)(y    ) + diff + nXScale;
			vertex[2].x = vertex[3].x = (double)(y + h) + diff;
			vertex[1].y = vertex[3].y = (double)(x    ) + diff + nYScale;
			vertex[0].y = vertex[2].y = (double)(x + w) + diff;
      break;

    case SR_90:
			vertex[1].x = vertex[3].x = (double)(y    ) + diff + nXScale;
			vertex[0].x = vertex[2].x = (double)(y + h) + diff;
			vertex[2].y = vertex[3].y = (double)(x    ) + diff + nYScale;
			vertex[0].y = vertex[1].y = (double)(x + w) + diff;
      break;
	  
    case SR_0: 
			vertex[0].x = vertex[2].x = (double)(x    ) + diff;
			vertex[1].x = vertex[3].x = (double)(x + w) + diff;
			vertex[0].y = vertex[1].y = (double)(y    ) + diff;
			vertex[2].y = vertex[3].y = (double)(y + h) + diff;
      break;
    }
 
 

	double rw = (double)w / (double)pw * (double)tw;
	double rh = (double)h / (double)ph * (double)th;

	vertex[0].u = vertex[2].u = (double)(px    ) / rw;
	vertex[1].u = vertex[3].u = (double)(px + w) / rw;
	vertex[0].v = vertex[1].v = (double)(py    ) / rh;
	vertex[2].v = vertex[3].v = (double)(py + h) / rh;
 

	// Z-buffer and RHW are unused for 2D blit, set to normal values

	vertex[0].z = vertex[1].z = vertex[2].z = vertex[3].z = 0.0f;
	vertex[0].rhw = vertex[1].rhw = vertex[2].rhw = vertex[3].rhw = 1.0f;

	if( g_createParams.orientation & ORIENTATION_FLIP_X )
    {
	  d3dvertex tmpVtx[4];

	  tmpVtx[0] = vertex[0];
	  tmpVtx[1] = vertex[1];
	  tmpVtx[2] = vertex[2];
	  tmpVtx[3] = vertex[3];

	  tmpVtx[0].x = vertex[3].x;
	  tmpVtx[1].x = vertex[2].x;
	  tmpVtx[2].x = vertex[1].x;
	  tmpVtx[3].x = vertex[0].x;

	  vertex[0] = tmpVtx[0];
	  vertex[1] = tmpVtx[1];
	  vertex[2] = tmpVtx[2];
	  vertex[3] = tmpVtx[3];
 
    }
    if( g_createParams.orientation & ORIENTATION_FLIP_Y )
    {
	  d3dvertex tmpVtx[4];

	  tmpVtx[0] = vertex[0];
	  tmpVtx[1] = vertex[1];
	  tmpVtx[2] = vertex[2];
	  tmpVtx[3] = vertex[3];

	  tmpVtx[0].y = vertex[3].y;
	  tmpVtx[1].y = vertex[2].y;
	  tmpVtx[2].y = vertex[1].y;
	  tmpVtx[3].y = vertex[0].y;

	  vertex[0] = tmpVtx[0];
	  vertex[1] = tmpVtx[1];
	  vertex[2] = tmpVtx[2];
	  vertex[3] = tmpVtx[3];
    }


	vertex[0].x += g_rendererOptions.m_screenOffsetX;
	vertex[0].y += g_rendererOptions.m_screenOffsetY;
	vertex[1].x += g_rendererOptions.m_screenOffsetX;
	vertex[1].y += g_rendererOptions.m_screenOffsetY;
	vertex[2].x += g_rendererOptions.m_screenOffsetX;
	vertex[2].y += g_rendererOptions.m_screenOffsetY;
	vertex[3].x += g_rendererOptions.m_screenOffsetX;
	vertex[3].y += g_rendererOptions.m_screenOffsetY;
 
#endif

	void *pLockedVertexBuffer;

	vertex[0].x = -1.0f;
	vertex[0].y = -1.0f;
	vertex[0].z = 0.0f;
	vertex[0].u = 0.0f;
	vertex[0].v = 1.0f;

	vertex[1].x = 1.0f;
	vertex[1].y = -1.0f;
	vertex[1].z = 0.0f;
	vertex[1].u = 1.0f;
	vertex[1].v = 1.0f;

	vertex[2].x = 1.0f;
	vertex[2].y = 1.0f;
	vertex[2].z = 0.0f;
	vertex[2].u = 1.0f;
	vertex[2].v = 0.0f;

	vertex[3].x = -1.0f;
	vertex[3].y = 1.0f;
	vertex[3].z = 0.0f;
	vertex[3].u = 0.0f;
	vertex[3].v = 0.0f;

	g_pD3DDevice->SetStreamSource(0,NULL,0,NULL);	
	HRESULT hr = g_pD3DVertexBuffer->Lock(0,0,&pLockedVertexBuffer,NULL);
	memcpy(pLockedVertexBuffer,vertex,sizeof(vertex));
	g_pD3DVertexBuffer->Unlock();
 
	return 0;
}
 
int VidSScaleImage(RECT* pRect, int nGameWidth, int nGameHeight)
{
	 

	int xm, ym;							// The multiple of nScrnWidth and nScrnHeight we can fit in
	int nScrnWidth, nScrnHeight;

	int nGameAspectX = g_createParams.aspect_x, nGameAspectY = g_createParams.aspect_y;
	int nWidth = pRect->right - pRect->left;
	int nHeight = pRect->bottom - pRect->top;
 
	xm = nWidth / nGameWidth;
	ym = nHeight / nGameHeight;

 
	 
		nScrnWidth = 1280.0;
		nScrnHeight = 720.0;
	 
	const float vidScrnAspect = (float)1280.0 / 720.0;

	 
		int nWidthScratch;
		nWidthScratch = nHeight * nGameAspectX * nScrnWidth / (nScrnHeight * vidScrnAspect * nGameAspectY);
		if (nWidthScratch > nWidth) {			// The image is too wide
			if (nGameWidth < nGameHeight) {		// Vertical games
				nHeight = nWidth * nGameAspectY * nScrnWidth / (nScrnHeight * vidScrnAspect * nGameAspectX);
			} else {							// Horizontal games
				nHeight = nWidth * vidScrnAspect * nGameAspectY * nScrnHeight / (nScrnWidth * nGameAspectX);
			}
		} else {
			nWidth = nWidthScratch;
		}
	 


 
	pRect->left = (pRect->right + pRect->left) / 2;
	pRect->left -= nWidth / 2;
	pRect->right = pRect->left + nWidth;
	pRect->top = (pRect->top + pRect->bottom) / 2;
	pRect->top -= nHeight / 2;
	pRect->bottom = pRect->top + nHeight;

	return 0;
}

int getClientScreenRect( RECT *pRect)
{
	POINT Corner = {0, 0};

	pRect->left = 0;
	pRect->right = 1280;
	pRect->top = 0;
	pRect->bottom = 720;

	return 0;
}

//-------------------------------------------------------------
//  CreateRenderingQuad
//-------------------------------------------------------------
static BOOL CreateRenderingQuad( void )
{
	
	
  if( g_pD3DVertexBuffer )
  {
    g_pD3DVertexBuffer->Release();
    g_pD3DVertexBuffer = NULL;
  }

 
   
	HRESULT Result;

	unsigned int vertexbuffersize =  sizeof(d3dvertex) << 2;

	psEffect = new ScalingEffect(g_pD3DDevice);
	psEffect->LoadEffect(pshader);
	
	Result = psEffect->Validate();
	
	if(psEffect->hasPreprocess())
	{
		vertexbuffersize = sizeof(d3dvertex) * 8;
		preProcess = true;
	}



	if (NULL == g_pD3DVertexBuffer)
	{
		Result = g_pD3DDevice->CreateVertexBuffer( vertexbuffersize ,D3DUSAGE_CPU_CACHED_MEMORY,0,0,&g_pD3DVertexBuffer,NULL);
		if(FAILED(Result)) {
			 
			return false;
		}
	}


	// Create vertex declaration
    if( NULL == g_pGradientVertexDecl )
    {
        static const D3DVERTEXELEMENT9 decl[] =
        {
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            D3DDECL_END()
        };

        if( FAILED( g_pD3DDevice->CreateVertexDeclaration( decl, &g_pGradientVertexDecl ) ) )
            return E_FAIL;
    }
 
 
	
	g_pD3DDevice->SetVertexDeclaration( g_pGradientVertexDecl );
 


	getClientScreenRect(&Dest);
	//VidSScaleImage(&Dest,g_createParams.width , g_createParams.height);

	int nNewImageWidth  = (Dest.right - Dest.left);
	int nNewImageHeight = (Dest.bottom - Dest.top);
 
	dx9SetVertex(g_textureRenderingArea.left,g_textureRenderingArea.top,g_textureRenderingArea.right, g_textureRenderingArea.bottom, nTextureWidth, nTextureHeight, Dest.left, Dest.top ,nNewImageWidth, nNewImageHeight);
 
	 
  
  g_pD3DDevice->SetStreamSource(	0,												  // Stream number
																	g_pD3DVertexBuffer,	0,				// Stream data
																	sizeof(d3dvertex) );		  // Vertex stride



  //g_pD3DDevice->SetRenderState(D3DRS_VIEWPORTENABLE, FALSE);
 
  return TRUE;
}



//---------------------------------------------------------------------
//	osd_save_snapshot
//---------------------------------------------------------------------


//============================================================
//	osd_override_snapshot
//============================================================

extern "C" struct mame_bitmap *osd_override_snapshot(struct mame_bitmap *bitmap, struct rectangle *bounds)
{
    /*
      Provides a hook to allow the OSD system to override processing of a
      snapshot.  This function will either return a new bitmap, for which the
      caller is responsible for freeing.
    */

  if( !(g_createParams.orientation & ORIENTATION_FLIP_Y) &&
      !(g_createParams.orientation & ORIENTATION_FLIP_X) &&
      !(g_createParams.orientation & ORIENTATION_SWAP_XY) )
  {
    return NULL;
  }
  else
  {
      //-- Taken from video.c in the windows distribution --------------------------------
	  struct rectangle newbounds;
	  struct mame_bitmap *copy;
	  int x, y, w, h, t;

	    // allocate a copy
	  w = (g_createParams.orientation & ORIENTATION_SWAP_XY) ? bitmap->height : bitmap->width;
	  h = (g_createParams.orientation & ORIENTATION_SWAP_XY) ? bitmap->width : bitmap->height;
	  copy = bitmap_alloc_depth(w, h, bitmap->depth);

	  if( !copy )
		  return NULL;

	    // populate the copy
	  for( y = bounds->min_y; y <= bounds->max_y; ++y)
    {
		  for( x = bounds->min_x; x <= bounds->max_x; ++x )
		  {
			  int tx = x, ty = y;

			    // apply the rotation/flipping
			  if ((g_createParams.orientation & ORIENTATION_SWAP_XY))
			  {
				  t = tx; tx = ty; ty = t;
			  }
			  if ((g_createParams.orientation & ORIENTATION_FLIP_X))
				  tx = copy->width - tx - 1;
			  if ((g_createParams.orientation & ORIENTATION_FLIP_Y))
				  ty = copy->height - ty - 1;

			    // read the old pixel and copy to the new location
			  switch (copy->depth)
			  {
				  case 15:
				  case 16:
					  *((UINT16 *)copy->base + ty * copy->rowpixels + tx) =
							  *((UINT16 *)bitmap->base + y * bitmap->rowpixels + x);
					  break;

				  case 32:
					  *((UINT32 *)copy->base + ty * copy->rowpixels + tx) =
							  *((UINT32 *)bitmap->base + y * bitmap->rowpixels + x);
					  break;
			  }
		  }
    }

	    // compute the oriented bounds
	  newbounds = *bounds;

	    // apply X/Y swap first
	  if ((g_createParams.orientation & ORIENTATION_SWAP_XY))
	  {
		  t = newbounds.min_x; newbounds.min_x = newbounds.min_y; newbounds.min_y = t;
		  t = newbounds.max_x; newbounds.max_x = newbounds.max_y; newbounds.max_y = t;
	  }

	    // apply X flip
	  if( (g_createParams.orientation & ORIENTATION_FLIP_X) )
	  {
		  t = copy->width - newbounds.min_x - 1;
		  newbounds.min_x = copy->width - newbounds.max_x - 1;
		  newbounds.max_x = t;
	  }

	    // apply Y flip
	  if ((g_createParams.orientation & ORIENTATION_FLIP_Y))
	  {
		  t = copy->height - newbounds.min_y - 1;
		  newbounds.min_y = copy->height - newbounds.max_y - 1;
		  newbounds.max_y = t;
	  }

	*bounds = newbounds;
    return copy;
  }

  return NULL;
}

