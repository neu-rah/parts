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
    template<typename T>
    inline auto operator()(T& o,Args... args)
      ->decltype(o.template map<API,Args...>(args...)) 
      {return o.template map<API,Args...>(args...);}
  };

  template<typename API,typename... Args>
  struct ForAll {
    template<typename T>
    inline void operator()(T& o,Args... args)
      {
        // _trace(clog<<"ForAll API"<<endl);
        o.template forAll<API,Args...>(args...);}
  };

  //Pair class
  template<typename First,typename Second>
  struct Pair {
    First _fst;
    Second _snd;
    using Fst=First;
    using Snd=Second;
    Fst& fst() {return _fst;}
    Snd& snd() {return _snd;}
    Pair() {}
    Pair(Fst f,Snd s):_fst(f),_snd(s) {}
    static constexpr Idx len() {return 1+Snd::len();}
    template<typename Func,Func fmap>
    auto map()
      -> decltype(Pair<decltype(fmap(fst())),decltype(snd().map(fmap))>(fmap(fst()),snd().map(fmap))) 
      {return Pair<decltype(fmap(fst())),decltype(snd().map(fmap))>(fmap(fst()),snd().map(fmap));}
    template<typename R>
    R fold(R(*f)(R,const Fst),R o) {return snd().fold(f,f(o,fst()));}
    template<typename API>
    void forAll() {
      API().operator()(fst());
      snd().template forAll<API>();
    }
  };
  template<typename First>
  struct Pair<First,void> {
    First _fst;
    using Fst=First;
    Fst& fst() {return _fst;}
    Pair() {}
    Pair(Fst f):_fst(f) {}
    static constexpr Idx len() {return 1;}
    template<typename Func,Func fmap>
    auto map()
      -> decltype(Pair<decltype(fmap(fst())),void>(fmap(fst()))) 
      {return Pair<decltype(fmap(fst())),void>(fmap(fst()));}
    template<typename R>
    R fold(R(*f)(R,const Fst),R o) {return f(o,fst());}
    template<typename API>
    void forAll() {API().operator()(fst());}
  };

  template<>
  struct Pair<void,void> {
    static constexpr Idx len() {return 0;}
    Pair() {}
    template<typename R>
    R fold(R(*)(R,R),R o) {return o;}
  };

  APIDEF(head,Head);
  APIDEF(tail,Tail);
  APIDEF(find,Find);
  // APIDEF(map,Map);
  // APIDEF(forAll,ForAll);

  //Node class
  //a pair exposing first element (head) API
  template<typename Fst,typename Snd>
  struct Node:Fst {
    using This=Node<Fst,Snd>;
    using Fst::Fst;
    // inline Node() {}
    template<typename... OO>
    Node(Fst f,OO... s):Fst(f),_tail(s...) {}
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
      {
        _trace(clog<<"find #"<<i<<endl;clog.flush());
        return API().operator()(head(),args...)?i:tail().template find<API,i+1,Args...>(args...);}

    template<typename API,typename... Args>
    auto map(Args... args) 
      ->Pair<
        decltype(API().operator()(head(),args...)),
        decltype(tail().template map<API,Args...>(args...))
      > {
        auto f=API().operator()(head(),args...);
        auto s=tail().template map<API,Args...>(args...);
      return Pair<
        decltype(API().operator()(head(),args...)),
        decltype(tail().template map<API,Args...>(args...))
        >(f,s);
    }

    template<typename API,typename... Args>
    void forAll(Args... args) {
      // _trace(clog<<"forall recursion..."<<endl);
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
          // _trace(clog<<"count down "<<(at-1)<<"\n\r");
          using T=typename Target::Tail::RefWalk;
          return T(pathLen,(Idx*)path).template step<typename Target::Tail,API,Args...>(at-1,target.tail(),args...);
        } else if(len()) {
          // _trace(clog<<"this path len:"<<len()<<"\n\r");
          using H=typename Target::Head::RefWalk;
          return H(pathLen-1,(Idx*)&path[1]).template step<typename Target::Head,API,Args...>(head(),target.head(),args...);
        }
          // _trace(clog<<"API call on head."<<"\n\r");
        return API().operator()(target.head(),args...);
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
  struct Node<Fst,Nil>:Fst {
    using Fst::Fst;
    using This=Node<Fst,Nil>;
    inline Node() {}
    inline Node(Fst f):Fst(f) {}
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
      {
        return Pair
          <decltype(API().operator()(self(),args...)), void>
          (API().operator()(self(), args...));
      }

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
        assert(!at);
        using H=typename Target::Head::RefWalk;
        return len()?
          H(pathLen-1,(Idx*)&path[1]).template step<typename Target::Head,API,Args...>(head(),target.head(),args...):
          API().operator()(target.head(),args...);
      }
    };

    template<typename Target,Idx o,Idx... oo>
    struct Walk {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args) 
        ->decltype(
          // o?
          // typename Target::Tail::template Walk<decltype(target.tail()),o-1,oo...>(target.tail()).template walk<API,Args...>(args...):
          typename Target::Head::template Walk<typename Target::Head,oo...>(target.head()).template walk<API,Args...>(args...)
        ) {
        // using T=typename Target::Tail::template Walk<decltype(target.tail()),o-1,oo...>;
        using H=typename Target::Head::template Walk<typename Target::Head,oo...>;
        // return o?
        //   T(target.tail()).template walk<API,Args...>(args...):
        return H(target.head()).template walk<API,Args...>(args...);
      }
    };

    template<typename Target,Idx o>
    struct Walk<Target,o> {
      Target& target;
      Walk(Target& t):target(t) {}
      template<typename API,typename... Args>
      auto walk(Args... args) 
        ->decltype(API().operator()(target.head(),args...))
        {assert(!o);return API().operator()(target.head(),args...);}
    };

    static constexpr Idx _sz() {return 1;}
  };

  template<typename O,typename... OO>
  struct TypeList:Pair<O,TypeList<OO...>>{
    using Base=Pair<O,TypeList<OO...>>;
    using This=TypeList<O,OO...>;
    using Base::Base;
    TypeList(O o,OO... oo):Base(o,TypeList(oo...)) {}
    typename Base::Fst head() {return Base::fst();}
    typename Base::Snd tail() {return Base::snd();}

    template<Idx n>
    auto staticDrop()
      ->decltype(n?tail().template staticDrop<n-1>():head()) 
      {return n?tail().template staticDrop<n-1>():head();}
  };
  template<typename O>
  struct TypeList<O>:Pair<O,void>{
    using Base=Pair<O,void>;
    using This=TypeList<O>;
    using Base::Base;
    TypeList(O o):Base(o) {}
    typename Base::Fst head() {return Base::fst();}
    template<Idx n>
    This staticDrop() {static_assert(!n);return *this;}
  };

};
