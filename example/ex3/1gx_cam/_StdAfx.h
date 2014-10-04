// Header files.
//
////////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER) && _MSC_VER > 1000
  #pragma once
  #pragma warning( disable : 4786)
  #pragma warning( disable : 4996)
#endif


#ifndef __StdAfx_H_
#define __StdAfx_H_


#if defined(_MSC_VER)
  #pragma comment(linker, "/subsystem:console")

  #pragma comment(lib, "lgpwn_b22_.lib")
  #pragma comment(lib, "lgpwn_c22_.lib")
  #pragma comment(lib, "lgpwn_x22_.lib")

  #pragma comment(lib, "ftf243m_.lib")

  #pragma comment(lib, "OpenGL32.lib")
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcMath.h>
#include <LcAux.h>
#include <LcStdev.h>
#include <LcxEntity.h>
#include <LcxMedia.h>


#include "AppGame.h"




#include "Main.h"


#endif

