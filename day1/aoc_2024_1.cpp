#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

void readFromFile(std::vector<int> &col1, std::vector<int> &col2) {
  std::ifstream file("aoc_2024_1.input");
  if (!file) {
    std::cerr << "Error: Could not open the file!" << std::endl;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    int value1, value2;
    if (iss >> value1 >> value2) {
      col1.push_back(value1);
      col2.push_back(value2);
    }
  }
}

long getDist(std::vector<int> col1, std::vector<int> col2) {
  std::sort(col1.begin(), col1.end());
  std::sort(col2.begin(), col2.end());

  long distance = 0;

  for (int i = 0; i < col1.size(); i++) {
    distance += abs(col1[i] - col2[i]);
  }

  return distance;
}

long getSimilarity(std::vector<int> &col1, std::vector<int> &col2) {
  long similarity = 0;
  for (int i = 0; i < col1.size(); i++) {
    similarity += col1[i] * std::count(col2.begin(), col2.end(), col1[i]);
  }

  return similarity;
}

int main() {
  std::vector<int> col1, col2;
  std::vector<int> test1{3, 4, 2, 1, 3, 3};
  std::vector<int> test2{4, 3, 5, 3, 9, 3};
  if (auto d = getDist(test1, test2) != 11) {
    throw std::runtime_error("Get distance, expecting 11 got " +
                             std::to_string(d));
  }

  if (auto d = getSimilarity(test1, test2) != 31) {
    throw std::runtime_error("Get similarity, expecting 31 got " +
                             std::to_string(d));
  }

  readFromFile(col1, col2);
  // Print column 2
  std::cout << "distance: " << getDist(col1, col2) << std::endl;
  std::cout << "similarity: " << getSimilarity(col1, col2) << std::endl;

  return 0;
}
