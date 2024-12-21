#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::vector<int>> readMatrixFromFile(const std::string &filename) {
  std::ifstream file(filename);
  std::vector<std::vector<int>> matrix;

  if (!file) {
    std::cerr << "Error: Could not open the file!" << std::endl;
    return matrix;
  }
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::vector<int> row;
    int value;
    while (iss >> value) {
      row.push_back(value);
    }
    matrix.push_back(row);
  }
  return matrix;
}

bool isSafe(const std::vector<int> &row) {
  bool increasing = false;
  bool safe = true;
  for (int i = 1; i < row.size(); i++) {
    if (row[i] > row[i - 1] && i == 1) {
      increasing = true;
    }
    if ((increasing &&
         ((row[i] - row[i - 1] < 1) || (row[i] - row[i - 1] > 3))) ||
        (!increasing &&
         ((row[i - 1] - row[i] < 1) || (row[i - 1] - row[i] > 3)))) {
      safe = false;
    }
  }
  return safe;
}

int getSafeCount(std::vector<std::vector<int>> input) {
  int safe = 0;
  bool increasing = false;
  bool safe_row;
  int r = 0;
  for (auto row : input) {
    safe_row = isSafe(row);
    if (safe_row) {
      ++safe;
    }
    r++;
  }
  return safe;
}

int getSafeCount2(std::vector<std::vector<int>> input) {
  int safe = 0;
  bool increasing = false;
  bool safe_row;
  int r = 0;
  for (auto row : input) {
    safe_row = true;
    increasing = false;
    bool toleration_already_activated = false;
    safe_row = isSafe(row);
    if (!safe_row) {
      // let's see what happens if we remove one of the elements
      for (int i = 0; i < row.size(); i++) {
        auto row_trial2 = row;
        row_trial2.erase(row_trial2.begin() + i);
        if (isSafe(row_trial2)) {
          ++safe;
          break;
        }
      }
    } else {
      ++safe;
    }
    r++;
  }
  return safe;
}

int main() {
  const std::vector<std::vector<int>> m = readMatrixFromFile("aoc_2024_2.test");
  int safeCount = getSafeCount(m);
  if (safeCount != 2) {
    throw std::runtime_error("Get Safe count, expecting 2 got " + safeCount);
  }

  safeCount = getSafeCount2(m);
  if (safeCount != 4) {
    throw std::runtime_error("Get Safe count, expecting 4 got " +
                             std::to_string(safeCount));
  }

  const std::vector<std::vector<int>> m2 =
      readMatrixFromFile("aoc_2024_2.input");
  std::cout << "safe count: " << getSafeCount(m2) << std::endl;
  std::cout << "safe with one tolaration count: " << getSafeCount2(m2)
            << std::endl;

  return 0;
}
