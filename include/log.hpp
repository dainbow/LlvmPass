#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

class Logger {
 public:
  struct Edge {
    std::string callee_;
    std::string caller_;

    uint64_t call_value_;
    uint64_t ret_value_;
  };

  static Logger& Instance();

  ~Logger();

  std::unordered_map<std::string, uint64_t> functions_;
  std::vector<Edge> edges_;
  std::stack<Edge> call_stack_;

 private:
  constexpr static double GoodFunctionH = 1. / 3.;
  constexpr static double CallThreshold = 20;

  void PrintFuncions(FILE* output, FILE* data_output);
  void PrintEdges(FILE* output, FILE* data_output);
};
