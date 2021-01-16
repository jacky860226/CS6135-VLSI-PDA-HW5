#pragma once
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

namespace GlobalTimer {
bool inited = false;
std::chrono::high_resolution_clock::time_point startTime;
std::chrono::seconds timeLimit;

void setTimeLimit(std::chrono::seconds seconds) {
  timeLimit = seconds;
  assert(inited == false);
  inited = true;
  startTime = std::chrono::high_resolution_clock::now();
}
template <class ToDuration = std::chrono::nanoseconds>
ToDuration getDuration() {
  auto endTime = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<ToDuration>(endTime - startTime);
}
bool overTime() { return inited && getDuration<>() >= timeLimit; }
}; // namespace GlobalTimer