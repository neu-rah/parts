#pragma once

#undef trace
#undef _trace
#undef __trace

#ifdef ARDUINO
  #include <Arduino.h>
  #ifdef CLOG_DEVICE
    #define clogDev CLOG_DEVICE
  #else 
    #define clogDev Serial
  #endif
  // using CLog=decltype(clog);
  #define endl "\n\r"
  struct CLog {
    enum Fmt:int {None=0,Hex=16,Dec=10,Oct=8,Bin=2};
    Fmt next=None;
    template<typename T> int print(T o) {
      Fmt f=next;
      next=None;
      return f==None?clogDev.print(o):clogDev.print(o,(int)f);
    }
    void flush() {clogDev.flush();}
  };
  extern CLog clog;
  template<> int CLog::template print<const char*>(const char* o) {return clogDev.print(o);}
  template<> int CLog::template print<CLog::Fmt>(CLog::Fmt fmt) {next=fmt;return 0;}
  CLog::Fmt hex=CLog::Fmt::Hex;
  constexpr CLog::Fmt dec=CLog::Fmt::Dec;
  constexpr CLog::Fmt oct=CLog::Fmt::Oct;
  constexpr CLog::Fmt bin=CLog::Fmt::Bin;

  template<typename T>
  CLog& operator<<(CLog& o,T t) {clog.print(t);return o;}
#else
  #include <iostream>
  #include <bitset>
  #ifdef CLOG_DEVICE
    #define clogDev CLOG_DEVICE
  #else
    #define clogDev (std::cout)
  #endif
  #define endl "\n\r"
//   enum CLogFmt {NO_FMT,HEX,DEC,OCT,BIN};
//   struct CLog {
//     enum Fmt:int {None=0,Hex=16,Dec=10,Oct=8,Bin=2};
//     template<typename T>
//     static int print(T& data,CLog::Fmt fmt=None) {
//       int st=clogDev.tellp();
//       switch(fmt) {
//         case HEX: clogDev.hex<<data;break;
//         case DEC: clogDev.dec<<data;break;
//         case OCT: clogDev.oct<<data;break;
//         case BIN: {
//           std::bitset< (sizeof(data)<<3) > d=data;
//           clogDev<<d;
//         }
//         break;
//         case NO_FMT:
//         default: clogDev<<data;break;
//       }
//       return ((int)(clogDev.tellp()))-st;
//     }
//     template<typename T>
//     static int println(T& data,CLogFmt fmt=None) {
//       int r=print(data,fmt);
//       clogDev<<std::endl;
//       return r;
//     }
//     static int write(char c) {clogDev.put(c);return 1;}
//     static int write(char* s,int n) {clogDev.write(s,n);return n;}
//   };
//   constexpr CLog clog;
//   template<typename T>
//   CLog& operator<<(CLog& o,T& p) {return o<<p;}
#endif

#ifdef RELEASE
  //disable all debug stuff on release
  #define trace(x)
  #define _trace(x)
  #define __trace(x)
  #define named(n)
#else
  #if defined(RS_DEBUG)
    // #include "menu/fmt/extra/ansiCodes.h"
    // using namespace ANSI;
    // #include <iostream>
    using namespace std;
    #define named(n) \
      static constexpr const char*name=n;\
      static constexpr const char* (named)() {return name;}\
      static void names() {\
        MDO::print(name);\
        MDO::print("/");\
        Base::names();\
      }
  #else
    #define named(n)
  #endif
  #if defined(RS_DEBUG) && defined(TRACE)
    #define trace(x) x
  #else
    #define trace(x)
  #endif
  #ifdef RS_DEBUG
    #define _trace(x) x
  #else
    #define _trace(x)
  #endif
  #define __trace(x) x
#endif
