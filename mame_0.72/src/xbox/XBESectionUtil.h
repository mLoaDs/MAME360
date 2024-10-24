/**
  * \file      XBESectionUtil.h
  * \brief     Registration of MAME files for creation and usage of XBOX
  *             loadable sections.
  *
  * \note      This file is autogenerated via Sectionize.pl DO NOT EDIT!
  */
#pragma once
//= I N C L U D E S ====================================================
#include "xbox_Mame.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "osd_cpu.h"
//= P R O T O T Y P E S ================================================

#ifdef _DEBUG
//-------------------------------------------------------------
//	CheckDriverSectionRAM
//! \brief    Prints the size of each driver/snd/vdeo segment
//-------------------------------------------------------------
void CheckDriverSectionRAM( void );

//-------------------------------------------------------------
//	CheckCPUSectionRAM
//! \brief    Prints the size of each CPU segment
//-------------------------------------------------------------
void CheckCPUSectionRAM( void );
#endif

//-------------------------------------------------------------
//	InitDriverSectionizer
//! \brief    Initializes the DriverSectionizer subsystem
//-------------------------------------------------------------
void InitDriverSectionizer( void );

//-------------------------------------------------------------
//	TerminateDriverSectionizer
//! \brief    Terminates the DriverSectionizer subsystem
//-------------------------------------------------------------
void TerminateDriverSectionizer( void );

//-------------------------------------------------------------
//	LoadDriverSectionByName
//! \brief    Loads the section associated with the passed name
//!
//! \param    DriverFileName - The name of the file whose section
//!                             should be loaded
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL LoadDriverSectionByName( const char *DriverFileName );

//-------------------------------------------------------------
//	UnloadDriverSectionByName
//! \brief    Unloads the section associated with the passed name
//!
//! \param    DriverFileName - The name of the file whose section
//!                             should be unloaded
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL UnloadDriverSectionByName( const char *DriverFileName );

//-------------------------------------------------------------
//	LoadDriverSections
//! \brief    Loads all of the driver sections
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL LoadDriverSections( void );

//-------------------------------------------------------------
//	UnloadDriverSections
//! \brief    Unloads all of the driver sections
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL UnloadDriverSections( void );


//-------------------------------------------------------------
//	InitCPUSectionizer
//! \brief    Initializes the DriverSectionizer subsystem
//-------------------------------------------------------------
void InitCPUSectionizer( void );

//-------------------------------------------------------------
//	TerminateCPUSectionizer
//! \brief    Terminates the DriverSectionizer subsystem
//-------------------------------------------------------------
void TerminateCPUSectionizer( void );

//-------------------------------------------------------------
//	LoadCPUSectionByID
//! \brief    Loads the section associated with the passed name
//!
//! \param    CPUID - The ID of the CPU whose section
//!                    should be loaded
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL LoadCPUSectionByID( UINT32 CPUID );

//-------------------------------------------------------------
//	UnloadCPUSectionByID
//! \brief    Unloads the section associated with the passed name
//!
//! \param    CPUID - The ID of the CPU whose section
//!                    should be loaded
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL UnloadCPUSectionByID( UINT32 CPUID );

//-------------------------------------------------------------
//	LoadCPUSections
//! \brief    Loads all of the CPU sections
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL LoadCPUSections( void );

//-------------------------------------------------------------
//	UnloadCPUSections
//! \brief    Unloads all of the CPU sections
//!
//! \return   BOOL - Operation status
//! \retval   TRUE - success
//! \return   FALSE - Failure
//-------------------------------------------------------------
BOOL UnloadCPUSections( void );
#ifdef __cplusplus
} // End extern "C"
#endif
