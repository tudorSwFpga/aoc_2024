#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using Position = std::tuple<int, int>;
using Graph = std::vector<std::vector<Position>>;

void read_input(const std::string &path, std::vector<std::string> &grid,
                std::string &movements) {
  bool isGrid = true;
  std::string line;
  std::ifstream file(path);

  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file");
  }

  while (std::getline(file, line)) {
    if (line.empty()) {
      isGrid = false; // Switch to reading movements after an empty line
      continue;
    }

    if (isGrid) {
      grid.push_back(line);
    } else {
      movements += line;
    }
  }

  file.close();
}

std::tuple<int, int> getFishPosition(const std::vector<std::string> &input) {
  std::tuple<int, int> ret;
  for (int row = 0; row < input.size(); row++) {
    for (int col = 0; col < input.at(row).size(); col++) {
      if (input.at(row)[col] == '@')
        ret = std::tuple<int, int>{row, col};
    }
  }
  return ret;
}

std::vector<std::string> resize_input(const std::vector<std::string> &grid) {
  std::vector<std::string> ret;
  for (int r = 0; r < grid.size(); r++) {
    std::string line = "";
    for (int c = 0; c < grid.at(r).size(); c++) {
      if (grid.at(r)[c] == '.' || grid.at(r)[c] == '#') {
        line += grid.at(r)[c];
        line += grid.at(r)[c];
      } else if (grid.at(r)[c] == '@') {
        line += grid.at(r)[c];
        line += '.';
      } else { // O case
        line += "[]";
      }
    }
    ret.push_back(line);
  }
  return ret;
}

bool moveObject(const Position &pos, const Position &dir,
                std::vector<std::string> &input) {
  int moves = 0;
  while (true) {
    int next_r = std::get<0>(pos) + moves * std::get<0>(dir);
    int next_c = std::get<1>(pos) + moves * std::get<1>(dir);
    if (input.at(next_r)[next_c] == '#') {
      return false;
    } else if (input.at(next_r)[next_c] == 'O') {
      ++moves;
    } else if (input.at(next_r)[next_c] == '.') {
      for (int i = 1; i <= moves; ++i) {
        next_r = std::get<0>(pos) + i * std::get<0>(dir);
        next_c = std::get<1>(pos) + i * std::get<1>(dir);
        input.at(next_r)[next_c] = 'O';
      }
      return true;
    }
  }
  return false;
}

bool canMoveFullCrateV(const bool &dirUp, int depth, const int &r, const int &c,
                       const std::vector<std::string> &input, Graph &graph) {
  const int r_offset = dirUp ? -1 : 1;
  const int c_offset = input.at(r).at(c) == '[' ? 1 : -1;
  if (graph.size() <= depth) {
    graph.push_back(std::vector<Position>());
  }
  graph.at(depth).push_back(Position(r, c));
  if (input.at(r)[c] == '.' && input.at(r)[c + c_offset] == '.') {
    return true;
  }
  if (input.at(r + r_offset)[c] == '.' &&
      input.at(r + r_offset)[c + c_offset] == '.') {
    return true;
  }
  if (input.at(r + r_offset)[c] == '#' ||
      input.at(r + r_offset)[c + c_offset] == '#') {
    return false;
  }

  if (input.at(r + r_offset)[c] != '.' &&
      input.at(r + r_offset)[c + c_offset] != '.') {
    return canMoveFullCrateV(dirUp, depth + 1, r + r_offset, c, input, graph) &&
           canMoveFullCrateV(dirUp, depth + 1, r + r_offset, c + c_offset,
                             input, graph);
  } else if (input.at(r + r_offset)[c] != '.') {
    return canMoveFullCrateV(dirUp, depth + 1, r + r_offset, c, input, graph);
  } else if (input.at(r + r_offset)[c + c_offset] != '.') {
    return canMoveFullCrateV(dirUp, depth + 1, r + r_offset, c + c_offset,
                             input, graph);
  }

  return false;
}

bool canMoveFullCrateH(const bool &dirLeft, int &depth, const int &r,
                       const int &c, const std::vector<std::string> &input) {
  const int c_offset = dirLeft ? -2 : 2;
  depth++;
  if (input.at(r)[c + c_offset] == '.') {
    return true;
  } else if (input.at(r)[c + c_offset] == '#') {
    return false;
  } else {
    return canMoveFullCrateH(dirLeft, depth, r, c + c_offset, input);
  }
}

void moveCrateVertically(const bool &dirUp, const Graph &g,
                         std::vector<std::string> input,
                         std::vector<std::string> &output) {
  output = input;
  const int r_offset = dirUp ? -1 : 1;
  for (auto i = g.size(); i > 0; --i) {
    for (auto j = 0; j < g.at(i - 1).size(); ++j) {
      const int r = std::get<0>(g.at(i - 1).at(j));
      const int c = std::get<1>(g.at(i - 1).at(j));
      const int c_offset = input.at(r)[c] == '[' ? 1 : -1;
      output.at(r + r_offset)[c] = input.at(r)[c];
      output.at(r + r_offset)[c + c_offset] = input.at(r)[c + c_offset];
      // if (i-1==0){
      output.at(r)[c] = '.';
      output.at(r)[c + c_offset] = '.';
      //}
    }
  }
  return;
}

void moveCrateHorizontally(const int &pos_r, const int &pos_c,
                           const Position &dir, const int &depth,
                           const std::vector<std::string> &input,
                           std::vector<std::string> &output) {
  output = input;
  if (std::get<1>(dir) == 1) {
    for (auto i = pos_c + 2 * depth; i > pos_c; --i) {
      output.at(pos_r)[i] = input.at(pos_r)[i - 1];
    }
  } else {
    for (auto i = pos_c - 2 * depth; i < pos_c; ++i) {
      output.at(pos_r)[i] = input.at(pos_r)[i + 1];
    }
  }
  output.at(pos_r)[pos_c] = '.';
  return;
}

bool moveObject_step2(const Position &pos, const Position &dir,
                      std::vector<std::string> &input) {
  const int r = std::get<0>(pos);
  const int c = std::get<1>(pos);
  bool moving_crate_vertically = false;
  if (dir == Position(1, 0) || dir == Position(-1, 0)) {
    Graph g;
    const bool dirUp = dir == Position(-1, 0);
    int depth = 0;
    if (canMoveFullCrateV(dirUp, depth, r, c, input, g)) {
      std::vector<std::string> output;
      moveCrateVertically(dirUp, g, input, output);
      input = output;
      return true;
    } else {
      return false;
    }
  } else {
    auto dir_left = dir == Position(0, -1);
    int depth = 0;
    if (canMoveFullCrateH(dir_left, depth, r, c, input)) {
      std::vector<std::string> output;
      moveCrateHorizontally(r, c, dir, depth, input, output);
      input = output;
      return true;
    } else {
      return false;
    }
  }
}

long long sum(const std::vector<std::string> &input, const std::string &step) {
  const char object = step == "1" ? 'O' : '[';
  long long res = 0;
  for (int row = 0; row < input.size(); row++) {
    for (int col = 0; col < input.at(row).size(); col++) {
      input.at(row)[col] == object ? res += 100 *row + col : res += 0;
    }
  }
  return res;
}

Position getDir(const char &d) {
  switch (d) {
  case '^': {
    return Position(-1, 0);
    break;
  }
  case '>': {
    return Position(0, 1);
    break;
  }
  case '<': {
    return Position(0, -1);
    break;
  }
  case 'v': {
    return Position(1, 0);
    break;
  }
  default: {
    break;
  }
  }
}

void printStep(std::vector<std::string> &input, const int &step,
               const char &movement) {
  std::cout << step << std::endl;
  for (int r = 0; r < input.size(); r++) {
    std::cout << input.at(r) << std::endl;
  }
  std::cout << std::endl;
}

std::string posToString(const std::tuple<int, int> &input) {
  return std::to_string(std::get<0>(input)) + ":" +
         std::to_string(std::get<1>(input));
}

long long step1(std::vector<std::string> &grid, const std::string &movements) {
  auto pos = getFishPosition(grid);
  for (int i = 0; i < movements.size(); i++) {
    printStep(grid, i, movements[i]);
    auto dir = getDir(movements[i]);
    auto next_r = std::get<0>(pos) + std::get<0>(dir);
    auto next_c = std::get<1>(pos) + std::get<1>(dir);
    // if next pos is not obstacle move else move
    if (grid.at(next_r)[next_c] == '#') {
      continue;
    } else if (grid.at(next_r)[next_c] == '.') {
      grid.at(std::get<0>(pos))[std::get<1>(pos)] = '.';
      grid.at(next_r)[next_c] = '@';
      pos = Position(next_r, next_c);
    } else if (moveObject(Position(next_r, next_c), dir, grid)) {
      grid.at(std::get<0>(pos))[std::get<1>(pos)] = '.';
      grid.at(next_r)[next_c] = '@';
      pos = Position(next_r, next_c);
    }
  }
  printStep(grid, movements.size(), movements[movements.size() - 1]);
  return sum(grid, "1");
}

long long step2(std::vector<std::string> &grid, const std::string &movements) {
  auto grid_resized = resize_input(grid);
  // auto grid_resized = grid;

  auto pos = getFishPosition(grid_resized);

  for (int i = 0; i < movements.size(); i++) {
    if (i == 94) {
      std::cout << "Here" << std::endl;
    }
    printStep(grid_resized, i, movements[i]);
    auto dir = getDir(movements[i]);
    auto next_r = std::get<0>(pos) + std::get<0>(dir);
    auto next_c = std::get<1>(pos) + std::get<1>(dir);
    // if next pos is not obstacle move else move
    if (grid_resized.at(next_r)[next_c] == '#') {
      continue;
    } else if (grid_resized.at(next_r)[next_c] == '.') {
      grid_resized.at(std::get<0>(pos))[std::get<1>(pos)] = '.';
      grid_resized.at(next_r)[next_c] = '@';
      pos = Position(next_r, next_c);
    } else if (moveObject_step2(Position(next_r, next_c), dir, grid_resized)) {
      grid_resized.at(std::get<0>(pos))[std::get<1>(pos)] = '.';
      grid_resized.at(next_r)[next_c] = '@';
      pos = Position(next_r, next_c);
    }
  }
  printStep(grid_resized, movements.size(), movements[movements.size() - 1]);
  return sum(grid_resized, "2");
}

int main(int argc, char *argv[]) {
  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }

  std::vector<std::string> grid;
  std::string movements;
  read_input(infile, grid, movements);
  if (step == "1") {
    std::cout << "Result = " << step1(grid, movements) << std::endl;
  }

  if (step == "2") {
    std::cout << "Result = " << step2(grid, movements) << std::endl;
  }
}
