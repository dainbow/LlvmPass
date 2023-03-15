#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

struct DynamicData {
  struct Edge {
    std::string from_;
    std::string to_;

    std::string label_;
  };

  DynamicData(std::ifstream& data_file);

  std::unordered_map<std::string, std::string> clusters_;
  std::vector<Edge> edges_;
};

void ModifyStatic(std::ifstream& static_data, DynamicData& dynamic_data);
