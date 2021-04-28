/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include <assert.h>
  #ifdef RS_DEBUG
    // #include <streamFlow.h>
    #include "debug.h"
  #endif
  using uint16_t=word;
#else
  #include <cstddef>
  #include <cstdint> 
  #include <cassert>
  using byte=unsigned char;
#endif

#include "../CRTPMix/CRTPMix.h"
using namespace CRTPMix;
// #include "menu/sys/kernel/debug.h"

