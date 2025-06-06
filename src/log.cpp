#include "../include/log.h"
#include <chrono>
#include <cstdio>
#include <print>

namespace Log {
  void Log(const std::string_view a){
    if(enabled){
       std::println(Log::fmtrstr, std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()), a);
       std::fflush(stdout);     
    }
  }
  bool enabled = false;
}
