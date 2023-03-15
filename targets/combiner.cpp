#include "../include/combiner.hpp"

int main() {
  std::ifstream dynamic_file("./data/dynamic_data.txt");
  DynamicData dynamic_data(dynamic_file);

  std::ifstream static_input("./graphs/static.txt");
  ModifyStatic(static_input, dynamic_data);
}

void ModifyStatic(std::ifstream& static_data, DynamicData& dynamic_data) {
  std::ofstream combined("./graphs/combined.txt");
  bool edges_printed = false;

  for (std::string first_word; std::getline(static_data, first_word, ' ');) {
    // std::cout << first_word << std::endl;
    if (first_word.find("subgraph") != std::string::npos) {
      // std::cout << "!!Subgraph detected!!" << std::endl;

      std::string cluster_name;
      std::getline(static_data, cluster_name, ' ');

      std::string remain;
      std::getline(static_data, remain);

      if (edges_printed == false) {
        combined << "itself[label = \"self-call\", style = hexagon]\n";

        for (auto& edge : dynamic_data.edges_) {
          if (edge.from_ != edge.to_) {
            combined << edge.from_ << " -> " << edge.to_ << "[label = \""
                     << edge.label_ << "\", lhead = cluster_" << edge.to_
                     << ", ltail = cluster_" << edge.from_ 
                     << "]\n";
          } else {
            combined << edge.from_ << " -> itself"
                     << "[label = \"" << edge.label_ << "\", ltail = cluster_"
                     << edge.from_ << "]" << std::endl;

            combined << "itself -> " << edge.from_ << "[lhead = cluster_"
                     << edge.from_ << "]" << std::endl;
          }
        }
        edges_printed = true;
      }

      combined << first_word << " " << cluster_name << " " << remain
               << std::endl;
      combined << "style = filled\n"
               << "fillcolor = \"" << dynamic_data.clusters_[cluster_name]
               << "\"\n";
    } else {
      std::string remain;
      std::getline(static_data, remain);

      combined << first_word << " " << remain << std::endl;
    }
  }
}

DynamicData::DynamicData(std::ifstream& data_file) {
  for (std::string data_type; std::getline(data_file, data_type, '$');) {
    if (data_type == "node") {
      std::string cluster_name;
      std::getline(data_file, cluster_name, '$');

      std::string cluster_color;
      std::getline(data_file, cluster_color);

      clusters_[cluster_name] = cluster_color;
    } else if (data_type == "edge") {
      std::string from;
      std::getline(data_file, from, '$');

      std::string to;
      std::getline(data_file, to, '$');

      std::string label;
      std::getline(data_file, label);

      edges_.push_back({from, to, label});
    }
  }
}
