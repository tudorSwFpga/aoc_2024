#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

using FuncArgs = std::pair<std::vector<long long>, long long>;
using Func = std::function<bool(const FuncArgs &)>;
using Coordinate = std::pair<int, int>;
using UniqueAntennaCoordinates = std::map<char, std::vector<Coordinate>>;

void read_input(const std::string &path, std::vector<std::string> &input) {
  // Read input from file
  std::ifstream infile(path);
  if (!infile) {
    throw std::runtime_error("Failed to open input file.");
  }

  std::string line;
  const char sep = ',';
  while (std::getline(infile, line)) {
    input.push_back(line);
  }

  infile.close();
}

void removeSingleAntennas(UniqueAntennaCoordinates &uniqueAntennaCoordinates) {
  for (auto it = uniqueAntennaCoordinates.begin();
       it != uniqueAntennaCoordinates.end();) {
    if (it->second.size() == 1) {
      std::cout << "Erasing unique" << std::endl;
      it = uniqueAntennaCoordinates.erase(it);
    } else {
      ++it;
    }
  }
}

void getAntennas(std::vector<std::string> &input,
                 UniqueAntennaCoordinates &uniqueAntennaCoordinates) {
  for (int r = 0; r < input.size(); r++) {
    for (int c = 0; c < input.at(r).size(); c++) {
      auto element = input.at(r)[c];
      if (element != '.') {
        // if (uniqueAntennaCoordinates.count(element)!=0){
        uniqueAntennaCoordinates[element].push_back(std::pair<int, int>(r, c));
        std::cout << "Found antenna " << element << " at " << r << ":" << c
                  << std::endl;
      }
    }
  }
  removeSingleAntennas(uniqueAntennaCoordinates);
}

int getAntinodesStep2(std::vector<std::string> &input,
                      UniqueAntennaCoordinates &uniqueAntennaCoordinates) {

  std::set<Coordinate> antinodes;

  for (auto antenas : uniqueAntennaCoordinates) {
    for (int i = 0; i < std::get<1>(antenas).size(); i++) {
      for (int j = i + 1; j < std::get<1>(antenas).size(); j++) {
        int r1 = std::get<0>(std::get<1>(antenas).at(i));
        int c1 = std::get<1>(std::get<1>(antenas).at(i));
        int r2 = std::get<0>(std::get<1>(antenas).at(j));
        int c2 = std::get<1>(std::get<1>(antenas).at(j));
        const int diff_r = r2 - r1;
        const int diff_c = c2 - c1;
        // if potential antinode not out of bounds, spawn it
        int n = 1;
        while (true) {
          if ((r1 - n * diff_r) >= 0 && (c1 - n * diff_c) >= 0 &&
              (r1 - n * diff_r) < input.size() &&
              (c1 - n * diff_c) < input.at(0).size()) {
            antinodes.emplace(
                std::pair<int, int>((r1 - n * diff_r), (c1 - n * diff_c)));
            input.at(r1 - n * diff_r)[c1 - n * diff_c] = '#';
            ++n;
          } else {
            n = 1;
            break;
          }
        }
        while (true) {
          if ((r2 + n * diff_r) >= 0 && (c2 + n * diff_c) >= 0 &&
              (r2 + n * diff_r) < input.size() &&
              (c2 + n * diff_c) < input.at(0).size()) {
            antinodes.emplace(
                std::pair<int, int>((r2 + n * diff_r), (c2 + n * diff_c)));
            input.at(r2 + n * diff_r)[c2 + n * diff_c] = '#';
            ++n;
          } else {
            n = 1;
            break;
          }
        }
      }
    }
    // now also add the antenna corordinates in the set
    for (auto coordinates : std::get<1>(antenas)) {
      antinodes.emplace(std::pair<int, int>(std::get<0>(coordinates),
                                            std::get<1>(coordinates)));
    }
  }

  return antinodes.size();
}

int getAntinodes(std::vector<std::string> &input,
                 UniqueAntennaCoordinates &uniqueAntennaCoordinates) {

  std::set<Coordinate> antinodes;

  for (auto antenas : uniqueAntennaCoordinates) {
    for (int i = 0; i < std::get<1>(antenas).size(); i++) {
      for (int j = i + 1; j < std::get<1>(antenas).size(); j++) {
        int r1 = std::get<0>(std::get<1>(antenas).at(i));
        int c1 = std::get<1>(std::get<1>(antenas).at(i));
        int r2 = std::get<0>(std::get<1>(antenas).at(j));
        int c2 = std::get<1>(std::get<1>(antenas).at(j));
        const int diff_r = r2 - r1;
        const int diff_c = c2 - c1;
        // if potential antinode not out of bounds, spawn it
        if ((r1 - diff_r) >= 0 && (c1 - diff_c) >= 0 &&
            (r1 - diff_r) < input.size() &&
            (c1 - diff_c) < input.at(0).size()) {
          antinodes.emplace(std::pair<int, int>((r1 - diff_r), (c1 - diff_c)));
          input.at(r1 - diff_r)[c1 - diff_c] = '#';
          ;
        }
        if ((r2 + diff_r) >= 0 && (c2 + diff_c) >= 0 &&
            (r2 + diff_r) < input.size() &&
            (c2 + diff_c) < input.at(0).size()) {
          antinodes.emplace(std::pair<int, int>((r2 + diff_r), (c2 + diff_c)));
          input.at(r2 + diff_r)[c2 + diff_c] = '#';
        }
      }
    }
  }
  return antinodes.size();
}

int main(int argc, char *argv[]) {
  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }
  // //step1+2 - test
  // {

  //     std::vector<std::string> input_test;
  //     std::vector<std::string> step1_test;
  //     std::vector<std::string> step2_test;
  //     read_input("aoc_2024_8.input.example",input_test);
  //     step1_test = input_test;
  //     step2_test = input_test;
  //     UniqueAntennaCoordinates uniqueAntennaCoordinates;
  //     getAntennas(input_test, uniqueAntennaCoordinates);
  //     auto result_s1 = getAntinodes(step1_test, uniqueAntennaCoordinates);
  //     auto result_s2 = getAntinodesStep2(step2_test,
  //     uniqueAntennaCoordinates); for (auto it:step1_test){
  //         std::cout << it << std::endl;
  //     }
  //     std::cout << "Result S1 = " << result_s1 << std::endl;

  //     for (auto it:step2_test){
  //         std::cout << it << std::endl;
  //     }
  //     std::cout << "Result S2 = " << result_s2 << std::endl;
  // }

  // step 1
  {
    std::vector<std::string> input;
    ;
    read_input(infile, input);
    auto input_modified = input;
    UniqueAntennaCoordinates uniqueAntennaCoordinates;
    getAntennas(input, uniqueAntennaCoordinates);
    if (step == "1") {
      auto result = getAntinodes(input_modified, uniqueAntennaCoordinates);
      for (auto it : input_modified) {
        std::cout << it << std::endl;
      }
      std::cout << "Result = " << result << std::endl;
    }
    if (step == "2") {
      auto result = getAntinodesStep2(input_modified, uniqueAntennaCoordinates);
      for (auto it : input_modified) {
        std::cout << it << std::endl;
      }
      std::cout << "Result = " << result << std::endl;
    }
  }
