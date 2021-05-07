/* -*- C++ -*- */
#pragma once

#include "path.h"
// #include "menu/item.h"

namespace Parts {

  // APIDEF(map,Map);
  // #define APIDEF(memberFunc,name)
  // template<typename... TAs>
  // struct Map {
  // template<typename T,typename... Args>
  // inline auto operator()(T& o,Args... args)
  //   ->decltype(o.template map<TAs...>(args...))
  //   {return o.template map<TAs...>(args...);}
  // };

  template<typename API,typename... Args>
  struct Map {
    constexpr static const char* named="Map";
    // _trace(MDO& operator<<(MDO& o) const {return o<<named<<"<"<<API()<<">";});
    template<typename T>
    inline auto operator()(T& o,Args... args)
      ->decltype(o.template map<API,Args...>(args...)) 
      {return o.template map<API,Args...>(args...);}
  };

  template<typename API,typename... Args>
  struct ForAll {
    constexpr static const char* named="Map";
    template<typename T>
    inline void operator()(T& o,Args... args)
      {o.template forAll<API,Args...>(args...);}
  };

  //Pair class
  template<typename Fst,typename Snd>
  struct Pair {
    Fst _head;
    Snd _tail;
    using Head=Fst;
    using Tail=Snd;
    Head& head() {return _head;}
    Tail& tail() {return _tail;}
    Pair(Fst f,Snd s):_head(f),_tail(s) {}
    Idx length() {return 1+tail().length();}
  };
  template<typename Fst>
  struct Pair<Fst,void> {
    Fst _head;
    Pair(Fst f):_head(f) {}
    static constexpr Idx length() {return 1;}
  };

  template<>
  struct Pair<void,void> {
    static constexpr Idx length() {return 1;}
  };

  //StaticList class
  //a pair exposing first element (head) API
  template<typename Fst,typename Snd>
  struct StaticList:Fst {
    using This=StaticList<Fst,Snd>;
    using Fst::Fst;
    // inline StaticList() {}
    template<typename... OO>
    StaticList(Fst f,OO... s):Fst(f),_tail(s...) {}
    Snd _tail;
    using Head=Fst;
    using Tail=Snd;
    Head& head() {return *this;}
    Tail& tail() {return _tail;}
    template<Idx n>
    auto drop()
      ->decltype(Drop<This,n>::drop(*this))
      {return Drop<This,n>::drop(*this);}

    template<typename API,int i,typename... Args>
    Idx find(Args... args) 
      {return API().operator()(head(),args...)?i:tail().template find<API,i+1,Args...>(args...);}

    template<typename API,typename... Args>
    auto map(Args... args) 
      ->Pair<
        decltype(API().operator()(head(),args...)),
        decltype(tail().template map<API,Args...>(args...))
      > {
      return Pair<
        decltype(API().operator()(head(),args...)),
        decltype(tail().template map<API,Args...>(args...))
        >(
        API().operator()(head(),args...),
        tail().template map<API,Args...>(args...)
      );
    }

    template<typename API,typename... Args>
    void forAll(Args... args) {
      API().operator()(head(),args...);
      tail().template forAll<API,Args...>(args...);
    }

    struct RefWalk {
      Idx pathLen;
      const Idx* path;
      RefWalk(Idx l,Idx* p):pathLen(l),path(p) {}
      Idx len() const {return pathLen;}
      Idx head() {return path[0];}
      template<typename Target,typename API,typename... Args>
      auto step(Idx at,Target& target,Args... args)
        ->decltype(
          at?
            typename Target::Tail::RefWalk(pathLen,(Idx*)path).template step<typename Target::Tail,API,Args...>(at-1,target.tail(),args...):
            len()?
              typename Target::Head::RefWalk(pathLen-1,(Idx*)&path[1]).template step<typename Target::Head,API,Args...>(head(),target.head(),args...):
              API().operator()(target,args...)
        ) {
        if(at) {
          using T=typename Target::Tail::RefWalk;
          return T(pathLen,(Idx*)path).template step<typename Target::Tail,API,Args...>(at-1,target.tail(),args...);
        } else if(len()) {
          using H=typename Target::Head::RefWalk;
          return H(pathLen-1,(Idx*)&path[1]).template step<typename Target::Head,API,Args...>(head(),target.head(),args...);
        } else return API().operator()(target.head(),args...);
      }
    };

    template<typename Target,Idx o,Idx... oo>
    struct Walk {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args)
        ->decltype(
            typename Tail::template Walk<typename Target::Tail,o-1,oo...>(target.tail()).template walk<API,Args...>(args...)
        ) {
        using T=typename Tail::template Walk<typename Target::Tail,o-1,oo...>;
        return T(target.tail()).template walk<API,Args...>(args...);
      }
    };

    template<typename Target,Idx... oo>
    struct Walk<Target,0,oo...> {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args)
        ->decltype(
            typename Head::template Walk<typename Target::Head,oo...>(target.head()).template walk<API,Args...>(args...)
        ) {
        using H=typename Head::template Walk<typename Target::Head,oo...>;
        return H(target.head()).template walk<API,Args...>(args...);
      }
    };

    template<typename Target,Idx o>
    struct Walk<Target,o> {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args) 
        ->decltype(
            typename Tail::template Walk<Tail,o-1>(target.tail())
              .template walk<API,Args...>(args...)
        ) {
        using TW=typename Tail::template Walk<Tail,o-1>;
        return TW(target.tail()).template walk<API,Args...>(args...);
      }
    };

    template<typename Target>
    struct Walk<Target,0> {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args)->decltype(API().operator()(target.head(),args...)) {
        // return Menu::Path<0>::template walk<Target,API,Args...>(target);
        return API().operator()(target,args...);
      }
    };

    static constexpr Idx _sz() {return Tail::_sz()+1;}

  };
  
  //list end
  template<typename Fst>
  struct StaticList<Fst,Nil>:Fst {
    using Fst::Fst;
    using This=StaticList<Fst,Nil>;
    inline StaticList() {}
    inline StaticList(Fst f):Fst(f) {}
    using Head=Fst;
    using Tail=Nil;
    Head& head() {return *this;}
    This& self() {return *this;}

    template<Idx n>
    auto drop()
      ->decltype(Drop<This,n>::drop(*this))
      {return Drop<This,n>::drop(*this);}

    template<typename API,int i,typename... Args>
    Idx find(Args... args) 
      {return API().operator()(head(),args...)?i:-1;}

    template<typename API,typename... Args>
    auto map(Args... args) 
      ->Pair<decltype(API().operator()(self(),args...)),void>
      {return Pair<decltype(API().operator()(self(),args...)),void>(API().operator()(self(),args...));}

    template<typename API,typename... Args>
    void forAll(Args... args) {API().operator()(*this,args...);}

    struct RefWalk {
      Idx pathLen;
      const Idx* path;
      RefWalk(Idx l,Idx* p):pathLen(l),path(p) {}
      RefWalk(const RefWalk& p):pathLen(p.len()),path(p.path) {}
      Idx len() const {return pathLen;}
      Idx head() {return path[0];}
      RefWalk tail() {return RefWalk(len()-1,(Idx*)&path[1]);}
      template<typename Target,typename API,typename... Args>
      auto step(Idx at,Target& target,Args... args) 
        ->decltype(
          len()?
            typename Target::Head::RefWalk(pathLen-1,(Idx*)&path[1]).template step<typename Target::Head,API,Args...>(head(),target.head(),args...):
            API().operator()(target,args...)
        ) {
        using H=typename Target::Head::RefWalk;
        return len()?
          H(pathLen-1,(Idx*)&path[1]).template step<typename Target::Head,API,Args...>(head(),target.head(),args...):
          API().operator()(target,args...);
      }
    };

    template<typename Target,Idx o,Idx... oo>
    struct Walk {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args) 
        ->decltype(o?
          typename Target::Tail::template Walk<decltype(target.tail()),o-1,oo...>(target.tail()).template walk<API,Args...>(args...):
          typename Target::Head::template Walk<typename Target::Head,oo...>(target.head()).template walk<API,Args...>(args...)
        ) {
        using T=typename Target::Tail::template Walk<decltype(target.tail()),o-1,oo...>;
        using H=typename Target::Head::template Walk<typename Target::Head,oo...>;
        return o?
          T(target.tail()).template walk<API,Args...>(args...):
          H(target.head()).template walk<API,Args...>(args...);
      }
    };

    template<typename Target,Idx o>
    struct Walk<Target,o> {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args) 
        ->decltype(API().operator()(target,args...))
        {assert(!o);return API().operator()(target,args...);}
    };

    static constexpr Idx _sz() {return 1;}
  };

};
