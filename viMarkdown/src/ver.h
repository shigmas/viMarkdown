//----------------------------------------------------------------------
//
//			File:			"ver.h"
//			Created:		19-Dec-2025
//			Author:			N.Tsuda
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#ifdef BUILD_WITH_CMAKE
#define		VER_STR			QString("ver 0.2.002 rc (CMake ver)")
#else
#define		VER_STR			QString("ver 0.2.002 rc")
#endif


