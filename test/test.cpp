#include <iostream>
#include <cassert>
using namespace std;

#include "../CRTPMix/CRTPMix.h"
using namespace CRTPMix;

#include "../src/parts.h"
using namespace Parts;

// using Cfg=Config<>;

// using Idx=typename Cfg::Idx;

// template<typename Target,Idx n>
// using Drop=PartsDef::Drop<Cfg,Target,n>;

// template<Idx... path>
// using Path=PartsDef::Path<Cfg,path...>;

// using PathRef=PartsDef::PathRef<Cfg>;

// template<typename Fst,typename Snd>
// using Pair=PartsDef::Pair<Cfg,Fst,Snd>;

// template<typename Fst,typename Snd>
// using StaticList=PartsDef::StaticList<Cfg,Fst,Snd>;

APIDEF(print,Print);

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

using Title=Item<StaticText<&title_text>::Part>;Title title;
using Body=Item<StaticText<&body_text>::Part>;Body body;
using Items=StaticList<Title,Body>;
Items items(title,body);

int main() {
  items.template drop<0>().head().print();
  // items.template map<Print>();
  return 0;
}