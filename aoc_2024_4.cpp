#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

class Mapp {
public:
  Mapp(const std::string &file_path) {
    m_matrix = readFile(file_path);
    m_nbRows = m_matrix.size();
    m_nbCols = m_matrix.at(0).size();
  }

  int getNumXmas() {
    m_XmasCnt = 0;
    for (int row = 0; row < m_nbRows; row++) {
      for (int col = 0; col < m_nbCols; col++) {
        // if not checked, let's check
        isXmas(row, col);
      }
    }
    return m_XmasCnt;
  }

  // appointement 2
  int getXmasX() {
    m_XmasCnt = 0;
    for (int row = 0; row < m_nbRows - 2; row++) {
      for (int col = 0; col < m_nbCols - 2; col++) {
        if ((m_matrix.at(row).at(col) == 'M' &&
             m_matrix.at(row + 1).at(col + 1) == 'A' &&
             m_matrix.at(row + 2).at(col + 2) == 'S' &&
             m_matrix.at(row + 2).at(col) == 'M' &&
             m_matrix.at(row).at(col + 2) == 'S') ||
            (m_matrix.at(row).at(col) == 'S' &&
             m_matrix.at(row + 1).at(col + 1) == 'A' &&
             m_matrix.at(row + 2).at(col + 2) == 'M' &&
             m_matrix.at(row + 2).at(col) == 'M' &&
             m_matrix.at(row).at(col + 2) == 'S') ||
            (m_matrix.at(row).at(col) == 'S' &&
             m_matrix.at(row + 1).at(col + 1) == 'A' &&
             m_matrix.at(row + 2).at(col + 2) == 'M' &&
             m_matrix.at(row + 2).at(col) == 'S' &&
             m_matrix.at(row).at(col + 2) == 'M') ||
            (m_matrix.at(row).at(col) == 'M' &&
             m_matrix.at(row + 1).at(col + 1) == 'A' &&
             m_matrix.at(row + 2).at(col + 2) == 'S' &&
             m_matrix.at(row + 2).at(col) == 'S' &&
             m_matrix.at(row).at(col + 2) == 'M')) {
          std::cout << "new MAS at " << row << " : " << col << std::endl;
          m_XmasCnt++;
        }
      }
    }
    return m_XmasCnt;
  }

  void printMatrix() {
    for (int i = 0; i < m_nbRows; i++) {
      std::vector<char> &row = m_matrix[i];
      for (int j = 0; j < m_nbCols; j++) {
        std::cout << row[j];
      }
      std::cout << "\n";
    }
  }

private:
  std::vector<std::vector<char>> m_matrix;
  std::tuple<int, int> m_direction = {0, 0};
  const std::array<std::tuple<int, int>, 8> m_all_dirs = {
      std::make_tuple(-1, -1), std::make_tuple(-1, 0), std::make_tuple(-1, 1),
      std::make_tuple(0, -1),  std::make_tuple(0, 1),  std::make_tuple(1, -1),
      std::make_tuple(1, 0),   std::make_tuple(1, 1)};
  const std::tuple<int, int> m_direction_pause = {0, 0};
  std::tuple<int, int> m_position_last = {0, 0};
  std::tuple<int, int> m_position_first_letter = {0, 0};
  int m_XmasCnt;
  int m_nbRows;
  int m_nbCols;
  char m_prevLetter;
  bool m_backwards;
  // not out of bounds
  bool inBounds(int r, int c) {
    return (r < m_nbRows && r >= 0) && (c < m_nbCols && c >= 0);
  }

  // not useful here
  bool isVisited(int r, int c) { return (m_matrix.at(r).at(c) != '.'); }

  bool isXmasStart(int r, int c) {
    if (m_matrix.at(r).at(c) == 'X') {
      m_backwards = false;
      return true;
    } else if (m_matrix.at(r).at(c) == 'S') {
      m_backwards = true;
      return true;
    }
    return false;
  }

  bool isNextLetter(const int &r, const int &c) {
    // const std::string fw = "XMAS";
    return (
        (m_matrix.at(r).at(c) == 'M' && m_prevLetter == 'X' && !m_backwards) ||
        (m_matrix.at(r).at(c) == 'A' && m_prevLetter == 'M' && !m_backwards) ||
        (m_matrix.at(r).at(c) == 'S' && m_prevLetter == 'A' && !m_backwards) ||
        (m_matrix.at(r).at(c) == 'A' && m_prevLetter == 'S' && m_backwards) ||
        (m_matrix.at(r).at(c) == 'M' && m_prevLetter == 'A' && m_backwards) ||
        (m_matrix.at(r).at(c) == 'X' && m_prevLetter == 'M' && m_backwards));
  }

  bool isSecondLetter(const int &r, const int &c) {
    // const std::string fw = "XMAS";
    return (
        (m_matrix.at(r).at(c) == 'M' && m_prevLetter == 'X' && !m_backwards) ||
        (m_matrix.at(r).at(c) == 'X' && m_prevLetter == 'M' && m_backwards));
  }

  bool isLastLetter(const int &r, const int &c) {
    // const std::string fw = "XMAS";
    return (
        (m_matrix.at(r).at(c) == 'S' && m_prevLetter == 'A' && !m_backwards) ||
        (m_matrix.at(r).at(c) == 'X' && m_prevLetter == 'M' && m_backwards));
  }

  bool isXmas(int r, int c) {
    // if it Xmas starts here or we're already in Xmas, keep digging
    if (m_direction == m_direction_pause && isXmasStart(r, c)) {
      m_position_first_letter = std::make_tuple(r, c);
      m_position_last = std::make_tuple(r, c);
      for (const auto &[dir_r, dir_c] : m_all_dirs) {
        m_prevLetter = m_matrix.at(r).at(c);
        if (inBounds(r + dir_r, c + dir_c)) {
          m_direction = std::make_tuple(dir_r, dir_c);
          isXmas(r + dir_r, c + dir_c);
        }
      }
      return false;
    } else if (m_direction != m_direction_pause && isNextLetter(r, c)) {
      if (isLastLetter(r, c)) {
        m_XmasCnt += 1;
        m_direction = m_direction_pause;
        m_prevLetter = m_matrix.at(r).at(c);
        return true;
      } else if (inBounds(r + std::get<0>(m_direction),
                          c + std::get<1>(m_direction))) {
        m_prevLetter = m_matrix.at(r).at(c);
        isXmas(r + std::get<0>(m_direction), c + std::get<1>(m_direction));
      } else {
        m_prevLetter = m_matrix.at(r).at(c);
        m_direction = m_direction_pause;
        return false;
      }
    } else {
      m_direction = m_direction_pause;
      return false;
    }
    return false;
  }

  std::vector<std::vector<char>> readFile(const std::string &file_path) {
    std::vector<std::vector<char>> matrix;
    // Open the file
    std::ifstream file(file_path);
    if (!file) {
      std::cerr << "Error: Unable to open file " << file_path << std::endl;
    }

    // Read the file line by line
    std::string line;
    while (std::getline(file, line)) {
      std::vector<char> row(line.begin(), line.end());
      matrix.push_back(row);
    }

    file.close();
    // Output the matrix to verify the result
    std::cout << "Matrix contents:" << std::endl;
    for (const auto &row : matrix) {
      for (char c : row) {
        std::cout << c << ' ';
      }
      std::cout << std::endl;
    }
    return matrix;
  }
};

int main() {
  Mapp xmasTestMap = Mapp("aoc_2024_4.test");
  auto res = xmasTestMap.getNumXmas() / 2;
  if (res != 18) {
    throw std::runtime_error("Expected 18 got " + std::to_string(res));
  }

  res = xmasTestMap.getXmasX();
  if (res != 9) {
    throw std::runtime_error("Expected 9 got " + std::to_string(res));
  }

  Mapp xmasRealMap = Mapp("aoc_2024_4.input");
  std::cout << "Result = " << xmasRealMap.getNumXmas() / 2;
  std::cout << "Result 2 = " << xmasRealMap.getXmasX();

  return 0;
}
