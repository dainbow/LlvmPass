#include "../include/log.hpp"

Logger& Logger::Instance() {
  static Logger singleton;

  return singleton;
}

Logger::~Logger() {
  FILE* output = fopen("./graphs/dynamic.txt", "w");
  assert(output);

  FILE* data_output = fopen("./data/dynamic_data.txt", "w");
  assert(data_output);

  fprintf(output, "digraph {\n");
  PrintFuncions(output, data_output);
  PrintEdges(output, data_output);
  fprintf(output, "}\n");

  fclose(data_output);
  fclose(output);
}

void Logger::PrintFuncions(FILE* output, FILE* data_output) {
  for (auto& func : functions_) {
    double H = (1 - std::min(1., double(func.second) / CallThreshold)) * GoodFunctionH;
    double S = 0.34;
    double V = 1;

    fprintf(data_output, "node$cluster_%s$%.3lf %.3lf %.3lf\n", func.first.c_str(), H, S, V);
    fprintf(output, "\t%s[color = \"%.3lf %.3lf %.3lf\", shape = box, style = filled];\n", func.first.c_str(), H, S, V);
  }
}

void Logger::PrintEdges(FILE* output, FILE* data_output) {
  std::unordered_map<
      std::string, std::unordered_map<
                       std::string, std::vector<std::pair<uint64_t, uint64_t>>>>
      callees;

  for (auto& edge : edges_) {
    callees[edge.callee_][edge.caller_].emplace_back(edge.call_value_,
                                                     edge.ret_value_);
  }

  for (auto& callee : callees) {
    for (auto& caller : callee.second) {
      fprintf(output, "\t%s -> %s [label = \" ", callee.first.c_str(),
              caller.first.c_str());
      fprintf(data_output, "edge$%s$%s$", callee.first.c_str(), caller.first.c_str());
      

      for (auto& edges : caller.second) {
        fprintf(output, "(%lu) = %lu; ", edges.first, edges.second);
        fprintf(data_output, "(%lu) = %lu; ", edges.first, edges.second);
      }

      fprintf(output, "\"]\n");
      fprintf(data_output, "\n");
    }
  }
}

extern "C" void _LogFuncEntry(const char* funcName) {
  Logger::Instance().functions_[funcName]++;
}

extern "C" void _LogRet(const uint64_t argument) {
  auto retEdge = std::move(Logger::Instance().call_stack_.top());
  Logger::Instance().call_stack_.pop();


  retEdge.ret_value_ = argument;
  Logger::Instance().edges_.emplace_back(std::move(retEdge));
}

extern "C" void _LogCall(const char* calleeName, const char* callerName,
                         const uint64_t argument) {
  Logger::Instance().call_stack_.push({calleeName, callerName, argument, 0});
}
