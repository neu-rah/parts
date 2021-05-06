#pragma once

#define APIDEF(memberFunc,name)\
struct name {\
  template<typename T,typename... Args>\
  inline auto operator()(T& o,Args... args)\
    ->decltype(o.memberFunc(args...))\
    {return o.memberFunc(args...);}\
};

#include "list.h"

// namespace Parts {
//   struct DefaultCfg {
//     #ifdef Arduino
//       using Idx=byte;
//     #else
//       using Idx=int;
//     #endif
//   };
//   template<typename Cfg=DefaultCfg>
//   struct Config {
//     using Idx=typename Cfg::Idx;

//     template<typename Target,Idx n>
//     using Drop=PartsDef::Drop<Cfg,Target,n>;

//     template<Idx... path>
//     using Path=PartsDef::Path<Cfg,path...>;

//     using PathRef=PartsDef::PathRef<Cfg>;

//     template<typename Fst,typename Snd>
//     using Pair=PartsDef::Pair<Cfg,Fst,Snd>;

//     template<typename Fst,typename Snd>
//     using StaticList=PartsDef::StaticList<Cfg,Fst,Snd>;

//   };
// };

