#include <iostream>
#include <cassert>
using namespace std;

#include "../CRTPMix/CRTPMix.h"
using namespace CRTPMix;

struct Nil {} nil;

template<typename O=Nil>
struct Empty {
  static void print() {}
};

template<template<typename> class... OO>
using Item=Composition<Empty,OO...>;

template<const char** text>
struct StaticText {
  template<typename O=Empty<>>
  struct Part:O {
    static void print() {cout<<*text;}
  };
};

const char* title_text="title";
const char* body_text="body";

using Idx=size_t;

template<typename F,typename S>
struct Tuple:F {
  using Fst=F;
  using Snd=S;
  // Fst fstValue;
  Snd sndValue;
  Tuple(Fst f,Snd s):F(f),sndValue(s) {}
  Fst fst() const {return *this;}
  Snd snd() const {return sndValue;}
};

// template<size_t n>
// struct If {
//   template<typename A,typename B> using IsZero=B;
//   static bool isZero() {return false;}
// };

// template<>
// struct If<0> {
//   template<typename A,typename B>using IsZero=A;
//   static bool isZero() {return true;}
// };

template<typename Target,Idx n>
struct Drop {
  static auto drop(const Target& target)
    ->decltype(Drop<typename Target::Tail,n-1>::drop(target.tail()))
    {return Drop<typename Target::Tail,n-1>::drop(target.tail());}
};
template<typename Target>
struct Drop<Target,0> {
  static Target drop(const Target& target) {return target;}
};

// struct DropRef {
//   using Idx=typename Cfg::Idx;
//   Idx pathLen;
//   const Idx* path;
//   DropRef(Idx l,Idx* p):pathLen(l),path(p) {}
//   Idx len() const {return pathLen;}
//   Idx head() {return path[0];}
// };


#define APIDef(f)\
struct f {\
  template<typename Target,typename... Args>\
  static auto run(Target& target,Args... args)\
    ->decltype(target.f(args...))\
    {return target.f(args...);}\
};

// APIDef(print);

struct Print {
  template<typename Target,typename... Args>
  static auto run(Target& target,Args... args)
    ->decltype(target.print(args...))
    {return target.print(args...);}
};

//empty static list
template<typename... OO>
struct List {
  using This=List<OO...>;
  static constexpr bool isEmpty() {return true;}
  static constexpr Idx length() {return 0;}
  constexpr This head() const {assert(false);return *this;}
  template<typename API,typename... Args>
  void call(Idx,Args...) {assert(false);}
  // static void print() {cout<<"wtf!<>";}
};

//static list
template<typename O,typename... OO>
struct List<O,OO...>:Tuple<O,List<OO...>> {
  using Base=Tuple<O,List<OO...>>;
  using This=List<O,OO...>;
  using Head=typename Base::Fst;
  using Tail=typename Base::Snd;
  // List() {}
  List(O o,OO... oo):Base(o,Tail(oo...)) {}
  Head head() const {return Base::fst();}
  Tail tail() const {return Base::snd();}
  template<Idx n>
  auto drop()
    ->decltype(Drop<This,n>::drop(*this))
    {return Drop<This,n>::drop(*this);}
  static constexpr bool isEmpty() {return false;}
  constexpr Idx length() {return 1+tail().length();}
  template<typename API,typename... Args>
  auto call(Idx n,Args... args)
    ->decltype(n?tail().template call<API,Args...>(n-1,args...):API::run(*this,args...))
    {return n?tail().template call<API,Args...>(n-1,args...):API::run(*this,args...);}
  // void print() {cout<<"wtf!<O,OO...>"<<endl;head().print();}
};

//single element static list
template<typename O>
struct List<O> {
  using This=List<O>;
  using Head=O;
  using Tail=List<>;
  O end;
  List(O o):end(o) {}
  Head head() const {return end;}
  Tail tail() const {return List<>();}
  static constexpr bool isEmpty() {return false;}
  static constexpr Idx length() {return 1;}
  template<typename API,typename... Args>
  auto call(Idx n,Args... args)
    ->decltype(API::run(*this,args...))
    {assert(!n);return API::run(*this,args...);}
  // static void print() {cout<<"wtf!<O>";}
};


using Title=Item<StaticText<&title_text>::Part>;Title title;
using Body=Item<StaticText<&body_text>::Part>;Body body;
using Items=List<Title,Body>;
Items items(title,body);

int main() {
  // cout<<"length:"<<items.length()<<endl;
  // items.head().print();cout<<endl;
  // items.drop<1>().head().print();cout<<endl;
  // cout<<[](auto x){return 2021-x;}(items.drop<0>().head())<<endl;
  // items.template call<Print>((Idx)0);
  // Print::template run<Items>(items.head());
  // Print::run(title);
  items.template call<Print>(0);
  cout<<endl;
  return 0;
}