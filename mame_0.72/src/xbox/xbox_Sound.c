/**
	* \file			xbox_Sound.c
	* \brief		Implementation of the "Sound" section of
	*           osdepend.h
  * \note     Ported from windows/sound.c
	*/

//= I N C L U D E S ====================================================
#include <xtl.h>
#include <xaudio2.h>
#include "osdepend.h"
#include "osd_cpu.h"
#include "driver.h"

#include <crtdbg.h>
#include "DebugLogger.h"
#include "xbox_Direct3DRenderer.h"



//= D E F I N E S ======================================================
  // Whether or not to enable sound debug messages
//#define LOG_SOUND

  // The number of audio updates to ignore underflow/overflows before
  //  reporting them
#define IGNORE_UNDERFLOW_FRAMES	100

  // the local buffer is what the stream buffer feeds from
  // note that this needs to be large enough to buffer at frameskip 11
  // for 30fps games like Tapper; we will scale the value down based
  // on the actual framerate of the game
#define MAX_BUFFER_SIZE			(128 * 1024)

  // this is the maximum number of extra samples we will ask for
  // per frame (I know this looks like a lot, but most of the
  // time it will generally be nowhere close to this)
#define MAX_SAMPLE_ADJUST		32

  //! The number of frames where we're in the [g_lowerThresh,g_upperThresh]
  //!  range before we reset the g_currentAdjustment value to 0
#define NUM_TARGET_FRAMES_BEFORE_ADJUST_RESET     10

//= G L O B A L = V A R S ==============================================

 
static INT32									g_attenuation = 0;

  // DirectSound objects
//static LPDIRECTSOUND8					g_pDSound = NULL;

  // sound buffers
//static LPDIRECTSOUNDBUFFER8		g_pStreamBuffer = NULL;
static UINT32									g_streamBufferSize = 0;
static UINT32				          g_streamBufferIn;

  // descriptors and formats
//static DSBUFFERDESC			      g_streamDesc = {0};
static WAVEFORMATEX			      g_streamFormat = {0};


// buffer over/underflow counts
static INT32									g_totalFrames = 0;
static INT32									g_bufferUnderflows = 0;
static INT32									g_bufferOverflows = 0;

	// global sample tracking
static INT									g_samplesPerFrame = 0;
static DOUBLE									g_samplesLeftOver = 0.0;
static UINT32									g_samplesThisFrame = 0;

	// sample rate adjustments
static INT32									g_currentAdjustment = 0;
static INT32									g_lowerThresh = 0;
static INT32									g_upperThresh = 0;

static BOOL                   g_soundPlaying = FALSE;


//= P R O T O T Y P E S ================================================
static BOOL Helper_XAudio2Initialize( void );
static void Helper_XAudio2Terminate( void );

static BOOL Helper_XAudio2CreateBuffers( void );
static void Helper_XAudio2DestroyBuffers( void );


static __inline UINT32 Helper_BytesInStreamBuffer( void );
static void Helper_UpdateSampleAdjustment( void );
static void Helper_CopySampleData( INT16 *data, UINT32 bytes_to_copy );
static void Helper_ClearSampleData( UINT32 amountToClear );


IXAudio2 *sound;
IXAudio2MasteringVoice* masterVoice;
IXAudio2SourceVoice* mixbuf;

UINT8 *locked_buf;
BYTE* pAudioBuffers;
int currentBuffer;
//= F U N C T I O N S ==================================================

//---------------------------------------------------------------------
//	osd_start_audio_stream
//---------------------------------------------------------------------
INT32 osd_start_audio_stream( INT32 stereo )
{
  //osd_start_audio_stream() is called at the start of the emulation to initialize
  //the output stream, then osd_update_audio_stream() is called every frame to
  //feed new data. osd_stop_audio_stream() is called when the emulation is stopped.

  //The sample rate is fixed at Machine->sample_rate. Samples are 16-bit, signed.
  //When the stream is stereo, left and right samples are alternated in the
  //stream.

  //osd_start_audio_stream() and osd_update_audio_stream() must return the number
  //of samples (or couples of samples, when using stereo) required for next frame.
  //This will be around Machine->sample_rate / Machine->drv->frames_per_second,
  //the code may adjust it by SMALL AMOUNTS to keep timing accurate and to
  //maintain audio and video in sync when using vsync. Note that sound emulation,
  //especially when DACs are involved, greatly depends on the number of samples
  //per frame to be roughly constant, so the returned value must always stay close
  //to the reference value of Machine->sample_rate / Machine->drv->frames_per_second.
  //Of course that value is not necessarily an integer so at least a +/- 1
  //adjustment is necessary to avoid drifting over time.

  // Disable the XBOX's WaitForVoiceOff warning
 
	locked_buf = NULL;

  g_totalFrames = 0;
  g_bufferUnderflows = 0;
  g_bufferOverflows = 0;

	  // determine the number of samples per frame    

    // Force it to 60 FPS, as we're always going to be running close to this value
  //if( g_rendererOptions.m_vsync )
  //  g_samplesPerFrame = (DOUBLE)Machine->sample_rate / 60.0;
  //else
  g_samplesPerFrame = (DOUBLE)Machine->sample_rate / (DOUBLE)Machine->drv->frames_per_second;

  PRINTMSG(( T_INFO, "Samples per frame: %f\n", g_samplesPerFrame ));
  PRINTMSG(( T_INFO, "Consumed per frame: %f\n", (DOUBLE)Machine->sample_rate / 60.0 ));
 
	// compute how many samples to generate the first frame
	g_samplesPerFrame++;
	g_samplesLeftOver = g_samplesPerFrame;
	g_samplesThisFrame = (UINT32)g_samplesLeftOver;
	g_samplesLeftOver -= (DOUBLE)g_samplesThisFrame;

	 // skip if sound disabled
	if( Machine->sample_rate )
	{
		// Initialize direct sound
		if( !Helper_XAudio2Initialize() )
			return 0;

		// set the startup volume
		osd_set_mastervolume( g_attenuation );
	}

	// return the samples to play the first frame
	return g_samplesThisFrame;
}

//---------------------------------------------------------------------
//	osd_stop_audio_stream
//---------------------------------------------------------------------
void osd_stop_audio_stream( void )
{
	// if nothing to do, don't do it
	if( !Machine->sample_rate )
		return;

	// kill the buffers and g_pDSound
	Helper_XAudio2Terminate();

  PRINTMSG(( T_INFO, "Sound buffer: overflows=%d underflows=%d\n", g_bufferOverflows, g_bufferUnderflows ));
}

//---------------------------------------------------------------------
//	osd_update_audio_stream
//---------------------------------------------------------------------
INT32 osd_update_audio_stream( INT16 *buffer )
{
  int original_bytes;
  int input_bytes;
  int final_bytes;
/*
static cycles_t lastFrameEndTime = 0;
cycles_t actualFrameCycles = osd_cycles() - lastFrameEndTime;
*/
	// if nothing to do, don't do it
	if( Machine->sample_rate   )
	{
 

    original_bytes = Helper_BytesInStreamBuffer();
    input_bytes = g_samplesThisFrame * g_streamFormat.nBlockAlign;

		  // update the sample adjustment
		Helper_UpdateSampleAdjustment();

		  // copy data into the sound buffer
		Helper_CopySampleData( buffer, input_bytes );

      // check for overflows
    final_bytes = Helper_BytesInStreamBuffer();
    if (final_bytes < original_bytes)
      ++g_bufferOverflows;

      // reset underflow/overflow tracking
    if (++g_totalFrames == IGNORE_UNDERFLOW_FRAMES)
      g_bufferOverflows = g_bufferUnderflows = 0;
    else if( g_totalFrames > IGNORE_UNDERFLOW_FRAMES )
      g_totalFrames = IGNORE_UNDERFLOW_FRAMES + 1;

    
	}

    // compute how many samples to generate next frame
	g_samplesLeftOver += g_samplesPerFrame;
	g_samplesThisFrame = (UINT32)g_samplesLeftOver;
	g_samplesLeftOver -= (DOUBLE)g_samplesThisFrame;

	g_samplesThisFrame += g_currentAdjustment;


/*
  PRINTMSG_TO_CONSOLE( T_NOPOSITION, "FPS %f SPF %3.3f SR: %lu", ((DOUBLE)osd_cycles_per_second() / (DOUBLE)actualFrameCycles), g_samplesPerFrame, Machine->sample_rate );
lastFrameEndTime = osd_cycles();
*/
	  // return the samples to play this next frame
	return g_samplesThisFrame;
}

//---------------------------------------------------------------------
//	osd_set_mastervolume
//---------------------------------------------------------------------
void osd_set_mastervolume( INT32 attenuation )
{
 // control master volume. attenuation is the attenuation in dB (a negative
 // number). To convert from dB to a linear volume scale do the following:
	//volume = MAX_VOLUME;
	//while (attenuation++ < 0)
	//	volume /= 1.122018454;		//	= (10 ^ (1/20)) = 1dB

	// clamp the attenuation to 0-32 range
	g_attenuation = attenuation;
  if( g_attenuation > 0 )
    g_attenuation = 0;
  else if( g_attenuation < -32 )
    g_attenuation = -32;

	// set the master volume
	//if( g_pStreamBuffer )
	//	IDirectSoundBuffer_SetVolume( g_pStreamBuffer, g_attenuation * 100 );
}

//---------------------------------------------------------------------
//	osd_get_mastervolume
//---------------------------------------------------------------------
INT32 osd_get_mastervolume( void )
{
	return g_attenuation;
}

//---------------------------------------------------------------------
//	osd_sound_enable
//---------------------------------------------------------------------
void osd_sound_enable( INT32 enable )
{
 
}

int loopLen;
int nAudAllocSegLen = 0;		// Allocated seg length in samples

//---------------------------------------------------------------------
//	Helper_DirectSoundInitialize
//---------------------------------------------------------------------
static BOOL Helper_XAudio2Initialize( void )
{
    

	// make a format description for what we want
	g_streamFormat.wFormatTag			= WAVE_FORMAT_PCM;
	g_streamFormat.nChannels			= (Machine->drv->sound_attributes & SOUND_SUPPORTS_STEREO) ? 2 : 1;
	g_streamFormat.nSamplesPerSec	= Machine->sample_rate;
	g_streamFormat.wBitsPerSample	= 16;
	g_streamFormat.nBlockAlign		= (g_streamFormat.wBitsPerSample * g_streamFormat.nChannels) >> 3;
	g_streamFormat.nAvgBytesPerSec	= g_streamFormat.nSamplesPerSec * g_streamFormat.nBlockAlign;

	// compute the buffer sizes
	g_streamBufferSize = ((UINT64)MAX_BUFFER_SIZE * (UINT64)g_streamFormat.nSamplesPerSec) / 44100;
	g_streamBufferSize = (g_streamBufferSize * g_streamFormat.nBlockAlign) / 4;
	g_streamBufferSize = (g_streamBufferSize * 30) / Machine->drv->frames_per_second;
	g_streamBufferSize = (g_streamBufferSize / 1024) * 1024;  // Drop remainder

	locked_buf = (BYTE *)malloc(g_samplesPerFrame);
	nAudAllocSegLen = g_samplesPerFrame << 2;
	loopLen = ((g_samplesPerFrame*4)<<2);

	pAudioBuffers = (BYTE *)malloc(loopLen);				// 4 buffers

	// compute the upper/lower thresholds
	g_lowerThresh = g_streamBufferSize / 5;
	g_upperThresh = (g_streamBufferSize << 1) / 5;
	
  #ifdef LOG_SOUND
	  PRINTMSG(( T_INFO, "stream_buffer_size = %d (max %d)\n", g_streamBufferSize, MAX_BUFFER_SIZE ));
	  PRINTMSG(( T_INFO, "lower_thresh = %d\n", g_lowerThresh ));
	  PRINTMSG(( T_INFO, "upper_thresh = %d\n", g_upperThresh ));
  #endif


    XAudio2Create(&sound, 0, XAUDIO2_DEFAULT_PROCESSOR );
	IXAudio2_CreateMasteringVoice(sound, &masterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL );

	IXAudio2_CreateSourceVoice(sound, &mixbuf, &g_streamFormat, XAUDIO2_VOICE_USEFILTER , 
								XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL );

	IXAudio2SourceVoice_Start(mixbuf, 0, 0);


	return TRUE;
}

//---------------------------------------------------------------------
//	Helper_DirectSoundTerminate
//---------------------------------------------------------------------
static void Helper_XAudio2Terminate( void )
{
	Helper_XAudio2DestroyBuffers();

 
}

//---------------------------------------------------------------------
//	Helper_DirectSoundCreateBuffers
//---------------------------------------------------------------------
static BOOL Helper_XAudio2CreateBuffers( void )
{
 
	return TRUE;
}

//---------------------------------------------------------------------
//	Helper_DirectSoundDestroyBuffers
//---------------------------------------------------------------------
static void Helper_XAudio2DestroyBuffers( void )
{
 
	if (mixbuf)
	{
		IXAudio2SourceVoice_DestroyVoice(mixbuf);
		mixbuf = NULL;
	}

	if (masterVoice)
	{
		IXAudio2MasteringVoice_DestroyVoice(masterVoice);
		masterVoice = NULL;
	}

	if (locked_buf)
	{
		free(locked_buf);
		locked_buf = NULL;
	}

	if (pAudioBuffers)
	{
		free(pAudioBuffers);
		pAudioBuffers = NULL;
	}
}


//---------------------------------------------------------------------
//	Helper_BytesInStreamBuffer
//---------------------------------------------------------------------
static __inline UINT32 Helper_BytesInStreamBuffer( void )
{
 
	return 0;
}


//---------------------------------------------------------------------
//	Helper_UpdateSampleAdjustment
//---------------------------------------------------------------------
static void Helper_UpdateSampleAdjustment( void )
{
 
}

//---------------------------------------------------------------------
//	Helper_CopySampleData
//---------------------------------------------------------------------
static void Helper_CopySampleData( INT16 *data, UINT32 totalToCopy )
{
 
 
	XAUDIO2_VOICE_STATE state;
	XAUDIO2_BUFFER xa2buffer={0};

	 			
	IXAudio2SourceVoice_GetState(mixbuf, &state, NULL);
	

 
    if (state.BuffersQueued > 3 )            
        return;
	 	
	
	memcpy(&pAudioBuffers[currentBuffer * nAudAllocSegLen], data, totalToCopy);
	xa2buffer.AudioBytes=totalToCopy;
	xa2buffer.pAudioData= &pAudioBuffers[currentBuffer * nAudAllocSegLen];
	xa2buffer.pContext=NULL;
	xa2buffer.Flags = XAUDIO2_END_OF_STREAM;

	IXAudio2SourceVoice_SubmitSourceBuffer(mixbuf, &xa2buffer, NULL);

	currentBuffer++;
	currentBuffer %= (4);
	 
	 
}



