#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu path reference, this kind of reference can be used by both dynamic and static content
*/

#include "base.h"
#include "debug.hpp"
namespace Parts {

  //static dropping from static list
  template<typename Target,Idx n>
  struct Drop {
    static auto drop(const Target& target)
      ->decltype(Drop<typename Target::Tail,n-1>::drop(target.tail()))
      {return Drop<typename Target::Tail,n-1>::drop(target.tail());}
  };
  //static dropping from static empty list
  template<typename Target>
  struct Drop<Target,0> {
    static Target drop(const Target& target) {return target;}
  };

  //static tree reference using positions chain
  template<Idx... path>
  struct Path {
    template<typename Target,typename API,typename... Args>
    static auto walk(Target& target,Args... args) 
      ->decltype(
        typename Target::template Walk<Target,path...>(target).template walk<API,Args...>(args...)
      ) {
      using TW=typename Target::template Walk<Target,path...>;
      return TW(target).template walk<API,Args...>(args...);
    }
  };

  //dynamic tree reference using positions chain
  struct PathRef {
    Idx pathLen;
    const Idx* path;
    PathRef(Idx l,Idx* p):pathLen(l),path(p) {}
    Idx len() const {return pathLen;}
    Idx head() {return path[0];}
    template<typename Target,typename API,typename... Args>
    auto walk(Target& target,Args... args) 
      ->decltype(
        len()?
          typename Target::RefWalk(pathLen-1,(Idx*)&path[1])
            .template step<Target,API,Args...>
            (head(),target,args...):
          API().operator()(target,args...)
      )
    {
      using T=typename Target::RefWalk;
      // _trace(clog<<"PathRef::walk len:"<<len()<<"\n\r");
      return len()?
        T(pathLen-1,(Idx*)&path[1])
          .template step<Target,API,Args...>
          (head(),target,args...):
        API().operator()(target,args...);
    }
    template<typename O>
    O& operator<<(O& out) const {
      out<<"{"<<(int)pathLen<<"|";
      for(Idx n=0;n<pathLen;n++) out<<(n?",":"")<<path[n];
      out<<"}";
      return out;
    }
  };
};
