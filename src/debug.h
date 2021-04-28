#pragma once

#undef trace
#undef _trace
#undef __trace

#ifdef RELEASE
  //disable all debug stuff on release
  #define trace(x)
  #define _trace(x)
  #define __trace(x)
  #define named(n)
#else
  #if defined(RS_DEBUG)
    #ifdef ARDUINO
      #ifdef CLOG_DEVICE
        #define clog CLOG_DEVICE
      #else 
        #define clog Serial
      #endif
      #define endl "\n\r"
      template<typename T>
      decltype(clog)& operator<<(decltype(clog)& o,T t) {clog.print(t);return o;}
    #else
      #ifdef CLOG_DEVICE
        #define clogDev CLOG_DEVICE
      #else
        #include <iostream>
        #include <bitset>
        #define clogDev (std::cout)
      #endif
      enum CLogFmt {NO_FMT,HEX,DEC,OCT,BIN};
      struct CLog {
        template<typename T>
        static int print(T& data,CLogFmt fmt=NO_FMT) {
          int st=clogDev.tellp();
          switch(fmt) {
            case HEX: clogDev.hex<<data;break;
            case DEC: clogDev.dec<<data;break;
            case OCT: clogDev.oct<<data;break;
            case BIN: {
              std::bitset< (sizeof(data)<<3) > d=data;
              clogDev<<d;
            }
            break;
            case NO_FMT:
            default: clogDev<<data;break;
          }
          return ((int)(clogDev.tellp()))-st;
        }
        template<typename T>
        static int println(T& data,CLogFmt fmt=NO_FMT) {
          int r=print(data,fmt);
          clogDev<<std::endl;
          return r;
        }
        static int write(char c) {clogDev.put(c);return 1;}
        static int write(char* s,int n) {clogDev.write(s,n);return n;}
      };
      constexpr CLog clog;
      template<typename T>
      CLog& operator<<(CLog& o,T& p) {return o<<p;}
    #endif
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
