#pragma once

#include "list.h"

namespace Parts {
  struct DefaultCfg {
    using Idx=int;
  };
  template<typename Cfg=DefaultCfg>
  struct Config {
    using Idx=typename Cfg::Idx;

    template<Idx... path>
    using Path=PartsDef::Path<Cfg,path...>;

    using PathRef=PartsDef::PathRef<Cfg>;

    template<typename Fst,typename Snd>
    using Pair=PartsDef::Pair<Cfg,Fst,Snd>;

    template<typename Fst,typename Snd>
    using StaticList=PartsDef::StaticList<Cfg,Fst,Snd>;

  };
};

