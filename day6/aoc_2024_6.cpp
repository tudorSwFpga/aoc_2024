#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

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

std::tuple<int, int> getAgentPosition(const std::vector<std::string> &input) {
  std::tuple<int, int> ret;
  for (int row = 0; row < input.size(); row++) {
    for (int col = 0; col < input.at(row).size(); col++) {
      if (input.at(row)[col] == '^' || input.at(row)[col] == '<' ||
          input.at(row)[col] == '>' || input.at(row)[col] == 'v')
        ret = std::tuple<int, int>{row, col};
    }
  }
  return ret;
}

std::tuple<int, int> getAgentDirection(const std::vector<std::string> &input,
                                       const int &r, const int &c) {
  for (int row = 0; row < input.size(); row++) {
    switch (input.at(r)[c]) {
    case '^':
      return std::tuple<int, int>{-1, 0};
    case '>':
      return std::tuple<int, int>{0, 1};
    case '<':
      return std::tuple<int, int>{0, -1};
    case 'v':
      return std::tuple<int, int>{1, 0};
    }
  }
  return std::tuple<int, int>();
}

void printAgentDirection(const std::tuple<int, int> &dir) {
  if (dir == std::tuple<int, int>{-1, 0})
    std::cout << '^';
  if (dir == std::tuple<int, int>{0, 1})
    std::cout << '>';
  if (dir == std::tuple<int, int>{0, -1})
    std::cout << '<';
  if (dir == std::tuple<int, int>{1, 0})
    std::cout << 'v';
}

char getAgent(const std::tuple<int, int> &dir) {
  if (dir == std::tuple<int, int>{-1, 0})
    return '^';
  if (dir == std::tuple<int, int>{0, 1})
    return '>';
  if (dir == std::tuple<int, int>{0, -1})
    return '<';
  if (dir == std::tuple<int, int>{1, 0})
    return 'v';
  return '.';
}

bool isNextInBounds(const std::vector<std::string> &input,
                    const std::tuple<int, int> &cur_pos,
                    const std::tuple<int, int> &dir) {
  const int nbCol = input.at(0).size();
  return (std::get<0>(cur_pos) + std::get<0>(dir) >= 0 &&
          std::get<0>(cur_pos) + std::get<0>(dir) < input.size() &&
          std::get<1>(cur_pos) + std::get<1>(dir) >= 0 &&
          std::get<1>(cur_pos) + std::get<1>(dir) < nbCol);
}

bool isNextObstacle(const std::vector<std::string> &input,
                    const std::tuple<int, int> &cur_pos,
                    const std::tuple<int, int> &dir) {
  return input.at(std::get<0>(cur_pos) +
                  std::get<0>(dir))[std::get<1>(cur_pos) + std::get<1>(dir)] ==
         '#';
}

void turnRight(std::vector<std::string> &input,
               const std::tuple<int, int> &cur_pos) {
  switch (input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)]) {
  case '^': {
    input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)] = '>';
    break;
  }
  case '>': {
    input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)] = 'v';
    break;
  }
  case '<': {
    input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)] = '^';
    break;
  }
  case 'v': {
    input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)] = '<';
    break;
  }
  default: {
    break;
  }
  }
}

void printStep(std::vector<std::string> &input,
               const std::tuple<int, int> &cur_pos,
               const std::tuple<int, int> &dir, const int &step) {
  std::cout << "Step -----------------" << step << std::endl;
  for (int r = 0; r < input.size(); r++) {
    for (int c = 0; c < input.at(r).size(); c++) {
      if (r == std::get<0>(cur_pos) && c == std::get<1>(cur_pos)) {
        printAgentDirection(dir);
      } else {
        std::cout << input.at(r)[c];
      }
    }
    std::cout << "\n";
  }
}

// we have a loop when curent position is the beginning of the trajectory and
// next position is 2nd position of the trajectory
bool isLoop(const std::vector<std::tuple<int, int>> &trajectory,
            const std::tuple<int, int> cur_pos) {
  auto begin = trajectory.begin();
  auto end = trajectory.begin();
  int idx = 0;
  std::vector<std::vector<std::tuple<int, int>>> substrings;
  for (const auto &it : trajectory) {
    if (it == cur_pos) {
      begin = end;
      end = trajectory.begin() + idx;
      if (std::distance(begin, end) != 0) {
        substrings.push_back(std::vector<std::tuple<int, int>>(begin, end));
        if (substrings.size() > 1) {
          for (int i = 1; i < substrings.size(); i++) {
            if (substrings.at(i) == substrings.at(i - 1)) {
              std::cout << "Found recurring " << std::get<0>(cur_pos) << ":"
                        << std::get<1>(cur_pos) << " at " << idx << std::endl;
              return true;
            }
          }
        }
      }
    }
    ++idx;
  }

  return false;
}

int getUniquePositions(std::vector<std::string> &input) {
  int nbSteps = 0;
  std::set<std::tuple<int, int>> visitedPlaces;
  const int nbCol = input.at(0).size();
  auto cur_pos = getAgentPosition(input);
  // iterate as long as not out of bounds
  auto dir =
      getAgentDirection(input, std::get<0>(cur_pos), std::get<1>(cur_pos));
  visitedPlaces.insert(cur_pos);
  while (true) {
    if (isNextInBounds(input, cur_pos, dir)) {
      if (isNextObstacle(input, cur_pos, dir)) {
        // so we can go forward unless there is an obstacle
        turnRight(input, cur_pos);
        dir = getAgentDirection(input, std::get<0>(cur_pos),
                                std::get<1>(cur_pos));
      } else {
        // go forward
        auto next_r = std::get<0>(cur_pos) + std::get<0>(dir);
        auto next_c = std::get<1>(cur_pos) + std::get<1>(dir);
        input.at(next_r)[next_c] = getAgent(dir);
        input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)] = '.';
        cur_pos = std::tuple<int, int>{next_r, next_c};
        visitedPlaces.insert(cur_pos);
        nbSteps++;
      }

    } else {
      break;
    }
    printStep(input, cur_pos, dir, nbSteps);
  }
  return visitedPlaces.size();
}

bool doesObstacleCauseLoop(std::vector<std::string> input) {
  int nbSteps = 0;
  int nbLoops = 0;
  std::vector<std::tuple<int, int>> visitedPlaces;
  const int nbCol = input.at(0).size();
  auto cur_pos = getAgentPosition(input);
  auto last_pos = cur_pos;
  // iterate as long as not out of bounds
  auto dir =
      getAgentDirection(input, std::get<0>(cur_pos), std::get<1>(cur_pos));
  visitedPlaces.push_back(cur_pos);
  while (true) {
    if (isNextInBounds(input, cur_pos, dir) &&
        !isLoop(visitedPlaces, cur_pos)) {
      if (isNextObstacle(input, cur_pos, dir)) {
        // so we can go forward unless there is an obstacle
        turnRight(input, cur_pos);
        dir = getAgentDirection(input, std::get<0>(cur_pos),
                                std::get<1>(cur_pos));
      } else {
        // go forward
        auto next_r = std::get<0>(cur_pos) + std::get<0>(dir);
        auto next_c = std::get<1>(cur_pos) + std::get<1>(dir);
        input.at(next_r)[next_c] = getAgent(dir);
        input.at(std::get<0>(cur_pos))[std::get<1>(cur_pos)] = '.';
        last_pos = cur_pos;
        cur_pos = std::tuple<int, int>{next_r, next_c};
        visitedPlaces.push_back(cur_pos);
        nbSteps++;
      }
    } else if (isLoop(visitedPlaces, cur_pos)) {
      return true;
    } else { // gets out
      return false;
    }
  }
  return false;
}

std::string posToString(const std::tuple<int, int> &input) {
  return std::to_string(std::get<0>(input)) + ":" +
         std::to_string(std::get<1>(input));
}

int main(int argc, char *argv[]) {
  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }

  std::vector<std::string> input;
  read_input(infile, input);
  if (step == "1") {
    std::cout << "Unique positions " << getUniquePositions(input) << std::endl;
  }
  if (step == "2") {
    int nbPossibleObstacles = 0;
    std::vector<std::tuple<int, int>> possibleObstacles;
    for (int r = 0; r < input.size(); r++) {
      for (int c = 0; c < input.size(); c++) {
        std::vector<std::string> input_step = input;
        input_step.at(r)[c] = '#';
        if (doesObstacleCauseLoop(input_step)) {
          nbPossibleObstacles++;
          possibleObstacles.push_back(std::tuple<int, int>{r, c});
        }
      }
    }
    std::cout << "Nb possible obstacles " << nbPossibleObstacles << std::endl;
    for (auto it : possibleObstacles) {
      std::cout << posToString(it) << std::endl;
    }
  }
