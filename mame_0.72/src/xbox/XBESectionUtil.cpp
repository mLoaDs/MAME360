
#include "xbox_Mame.h"
#include <stdio.h>
#include <map>
#include <string>
#include "DebugLogger.h"


extern "C" {
#include "osd_cpu.h"
#include "cpuintrf.h"
}


//= D E F I N E S ======================================================

struct driverSectionRegistration_t 
{ 
	char	m_driverName[16];
	char	m_sectionName[8];
};


#define REGISTER_DRIVERSECTION( driverName, sectionName )     { driverName, sectionName }
 
//= G L O B A L = V A R S ==============================================
static std::map< UINT, std::string >         g_CPUIDToSectionMap;

static BOOL																	 g_driverSectionizerLoaded = TRUE;
static BOOL																	 g_cpuSectionizerLoaded = TRUE;

//= P R O T O T Y P E S ================================================
extern "C" static void RegisterCPUSectionNames( void );


//= F U N C T I O N S ==================================================
extern "C" {

//-------------------------------------------------------------
//	InitDriverSectionizer
//-------------------------------------------------------------
void InitDriverSectionizer( void )
{
	g_driverSectionizerLoaded = TRUE;
}


//-------------------------------------------------------------
//	TerminateDriverSectionizer
//-------------------------------------------------------------
void TerminateDriverSectionizer( void )
{
	g_driverSectionizerLoaded = FALSE;
}

//-------------------------------------------------------------
//	InitCPUSectionizer
//-------------------------------------------------------------
void InitCPUSectionizer( void )
{
  g_CPUIDToSectionMap.clear();
  g_cpuSectionizerLoaded = TRUE;
  RegisterCPUSectionNames();
}

//-------------------------------------------------------------
//	TerminateCPUSectionizer
//-------------------------------------------------------------
void TerminateCPUSectionizer( void )
{
  g_CPUIDToSectionMap.clear();
  g_cpuSectionizerLoaded = FALSE;
}
 


//-------------------------------------------------------------
//	IsDriverClone
//-------------------------------------------------------------
inline BOOL IsDriverClone( const char *driverName )
{
  return( !strcmp( driverName, "40love.c" ) ||
        !strcmp( driverName, "8080bw.c" ) ||
        !strcmp( driverName, "rotaryf.c" ) ||
        !strcmp( driverName, "aburner.c" ) ||
        !strcmp( driverName, "taito_b.c" ) ||
        !strcmp( driverName, "taito_f3.c" ) ||
        !strcmp( driverName, "groundfx.c" ) ||
        !strcmp( driverName, "gunbustr.c" ) ||
        !strcmp( driverName, "outrun.c" ) ||
        !strcmp( driverName, "segasyse.c" ) ||
        !strcmp( driverName, "system18.c" ) ||
        !strcmp( driverName, "segac2.c" ) ||
        !strcmp( driverName, "sharrier.c" ) ||
        !strcmp( driverName, "superchs.c" ) ||
        !strcmp( driverName, "undrfire.c" ) ||
        !strcmp( driverName, "battlera.c" ) ||
        !strcmp( driverName, "cbuster.c" ) ||
        !strcmp( driverName, "cninja.c" ) ||
        !strcmp( driverName, "darkseal.c" ) ||
        !strcmp( driverName, "dassault.c" ) ||
        !strcmp( driverName, "dec0.c" ) ||
        !strcmp( driverName, "deco32.c" ) ||
        !strcmp( driverName, "funkyjet.c" ) ||
        !strcmp( driverName, "madmotor.c" ) ||
        !strcmp( driverName, "rohga.c" ) ||
        !strcmp( driverName, "supbtime.c" ) ||
        !strcmp( driverName, "tumblep.c" ) ||
        !strcmp( driverName, "vaportra.c" ) ||
        !strcmp( driverName, "cclimber.c" ) ||
        !strcmp( driverName, "cvs.c" ) ||
        !strcmp( driverName, "dkong.c" ) ||
        !strcmp( driverName, "epos.c" ) ||
        !strcmp( driverName, "galaxian.c" ) ||
        !strcmp( driverName, "ladybug.c" ) ||
        !strcmp( driverName, "mario.c" ) ||
        !strcmp( driverName, "pengo.c" ) ||
        !strcmp( driverName, "phoenix.c" ) ||
        !strcmp( driverName, "frogger.c" ) ||
        !strcmp( driverName, "pacman.c" ) ||
        !strcmp( driverName, "scobra.c" ) ||
        !strcmp( driverName, "scramble.c" ) ||
        !strcmp( driverName, "amidar.c" ) ||
        !strcmp( driverName, "malzak.c" ) ||
        !strcmp( driverName, "yamato.c" ) ||
        !strcmp( driverName, "arcadecl.c" ) ||
        !strcmp( driverName, "btoads.c" ) ||
        !strcmp( driverName, "coolpool.c" ) ||
        !strcmp( driverName, "gottlieb.c" ) ||
        !strcmp( driverName, "exterm.c" ) ||
        !strcmp( driverName, "harddriv.c" ) ||
        !strcmp( driverName, "kinst.c" ) ||
        !strcmp( driverName, "lethalj.c" ) ||
        !strcmp( driverName, "williams.c" ) ||
        !strcmp( driverName, "midyunit.c" ) ||
        !strcmp( driverName, "midtunit.c" ) ||
        !strcmp( driverName, "mcr3.c" ) ||
        !strcmp( driverName, "mcr68.c" ) ||
        !strcmp( driverName, "mcr1.c" ) ||
        !strcmp( driverName, "mcr2.c" ) ||
        !strcmp( driverName, "midvunit.c" ) ||
        !strcmp( driverName, "midwunit.c" ) ||
        !strcmp( driverName, "midxunit.c" ) ||
        !strcmp( driverName, "seattle.c" ) ||
        !strcmp( driverName, "tickee.c" ) ||
        !strcmp( driverName, "asuka.c" ) ||
        !strcmp( driverName, "opwolf.c" ) ||
        !strcmp( driverName, "rainbow.c" ) ||
        !strcmp( driverName, "topspeed.c" ) ||
        !strcmp( driverName, "ataxx.c" ) ||
        !strcmp( driverName, "kncljoe.c" ) ||
        !strcmp( driverName, "m62.c" ) ||
        !strcmp( driverName, "mpatrol.c" ) ||
        !strcmp( driverName, "namcos1.c" ) ||
        !strcmp( driverName, "namcos86.c" ) ||
        !strcmp( driverName, "pacland.c" ) ||
        !strcmp( driverName, "skykid.c" ) ||
        !strcmp( driverName, "namcoic.c" ) ||
        !strcmp( driverName, "namcos2.c" ) ||
        !strcmp( driverName, "tceptor.c" ) ||
        !strcmp( driverName, "namconb1.c" ) ||
        !strcmp( driverName, "namcos21.c" ) ||
        !strcmp( driverName, "travrusa.c" ) ||
        !strcmp( driverName, "troangel.c" ) ||
        !strcmp( driverName, "yard.c" ) ||
        !strcmp( driverName, "batman.c" ) ||
        !strcmp( driverName, "klax.c" ) ||
        !strcmp( driverName, "eprom.c" ) ||
        !strcmp( driverName, "vindictr.c" ) ||
        !strcmp( driverName, "battlane.c" ) ||
        !strcmp( driverName, "bigstrkb.c" ) ||
        !strcmp( driverName, "cischeat.c" ) ||
        !strcmp( driverName, "bwidow.c" ) ||
        !strcmp( driverName, "chinagat.c" ) ||
        !strcmp( driverName, "clshroad.c" ) ||
        !strcmp( driverName, "cps2.c" ) ||
        !strcmp( driverName, "playmark.c" ) ||
        !strcmp( driverName, "sslam.c" ) ||
        !strcmp( driverName, "exidy.c" ) ||
        !strcmp( driverName, "fcombat.c" ) ||
        !strcmp( driverName, "gaelco.c" ) ||
        !strcmp( driverName, "galpani2.c" ) ||
        !strcmp( driverName, "goal92.c" ) ||
        !strcmp( driverName, "hal21.c" ) ||
        !strcmp( driverName, "jcross.c" ) ||
        !strcmp( driverName, "marvins.c" ) ||
        !strcmp( driverName, "sgladiat.c" ) ||
        !strcmp( driverName, "hyperspt.c" ) ||
        !strcmp( driverName, "sbasketb.c" ) ||
        !strcmp( driverName, "yiear.c" ) ||
        !strcmp( driverName, "junofrst.c" ) ||
        !strcmp( driverName, "timeplt.c" ) ||
        !strcmp( driverName, "rallyx.c" ) ||
        !strcmp( driverName, "locomotn.c" ) ||
        !strcmp( driverName, "pooyan.c" ) ||
        !strcmp( driverName, "rocnrope.c" ) ||
        !strcmp( driverName, "konamigq.c" ) ||
        !strcmp( driverName, "mystwarr.c" ) ||
        !strcmp( driverName, "m90.c" ) ||
        !strcmp( driverName, "shisen.c" ) ||
        !strcmp( driverName, "vigilant.c" ) ||
        !strcmp( driverName, "m92.c" ) ||
        !strcmp( driverName, "marineb.c" ) ||
        !strcmp( driverName, "zodiack.c" ) ||
        !strcmp( driverName, "missb2.c" ) ||
        !strcmp( driverName, "ms32.c" ) ||
        !strcmp( driverName, "multi32.c" ) ||
        !strcmp( driverName, "nbmj8891.c" ) ||
        !strcmp( driverName, "pipedrm.c" ) ||
        !strcmp( driverName, "pkunwar.c" ) ||
        !strcmp( driverName, "rollrace.c" ) ||
        !strcmp( driverName, "scregg.c" ) ||
        !strcmp( driverName, "sega.c" ) ||
        !strcmp( driverName, "spiders.c" ) ||
        !strcmp( driverName, "stv.c" ) ||
        !strcmp( driverName, "taito_x.c" ) ||
        !strcmp( driverName, "tmnt.c" ) ||
        !strcmp( driverName, "toypop.c" ) ||
        !strcmp( driverName, "wardner.c" ) ||
        !strcmp( driverName, "warriorb.c" ) ||
        !strcmp( driverName, "wc90b.c" ) );
}
 
//-------------------------------------------------------------
//	LoadDriverSections
//-------------------------------------------------------------
BOOL LoadDriverSections( void )
{
 
  return TRUE;
}

//-------------------------------------------------------------
//	UnloadDriverSections
//-------------------------------------------------------------
BOOL UnloadDriverSections( void )
{
	return TRUE;
}

//-------------------------------------------------------------
//	LoadDriverSectionByName
//-------------------------------------------------------------
BOOL LoadDriverSectionByName( const char *driverFileName )
{
  return TRUE;
}


//-------------------------------------------------------------
//	UnloadDriverSectionByName
//-------------------------------------------------------------
BOOL UnloadDriverSectionByName( const char *driverFileName )
{
  return TRUE;
}
 

//-------------------------------------------------------------
//	RegisterSectionID
//-------------------------------------------------------------
static void RegisterSectionID( UINT32 CPUID, const char *DataSectionName )
{
    // Add the section name to the map
  g_CPUIDToSectionMap[ CPUID ] = DataSectionName;
}


//-------------------------------------------------------------
//	LoadCPUSectionByID
//-------------------------------------------------------------
BOOL LoadCPUSectionByID( UINT32 CPUID )
{
 
  return TRUE;
}


//-------------------------------------------------------------
//	UnloadCPUSectionByID
//-------------------------------------------------------------
BOOL UnloadCPUSectionByID( UINT32 CPUID )
{
	return TRUE;
}


//-------------------------------------------------------------
//	LoadCPUSections
//-------------------------------------------------------------
BOOL LoadCPUSections( void )
{
 
  return TRUE;
}


//-------------------------------------------------------------
//	UnloadCPUSections
//-------------------------------------------------------------
BOOL UnloadCPUSections( void )
{
 
  return TRUE;
}


//-------------------------------------------------------------
//	RegisterCPUSectionNames
//-------------------------------------------------------------
static void RegisterCPUSectionNames( void )
{
 
}
 
} // End extern "C"


