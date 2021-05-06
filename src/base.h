/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include <assert.h>
  #ifdef RS_DEBUG
    // #include <streamFlow.h>
    #include "debug.h"
  #endif
  using uint16_t=word;
  using Idx=word;
#else
  #include <cstddef>
  #include <cstdint> 
  #include <cassert>
  using byte=unsigned char;
  using Idx=uint16_t;
#endif

#include "../CRTPMix/CRTPMix.h"
using namespace CRTPMix;

//the empty API
namespace Parts {
  struct Nil {
    using This=Nil;
  };
};
