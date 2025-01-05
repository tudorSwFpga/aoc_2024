#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

using Coordinate = std::pair<int, int>;
// trail = a start and an end
using Trail = std::pair<Coordinate, Coordinate>;
using FullTrail = std::vector<Coordinate>;
using UniqueTrails = std::set<Trail>;
using UniqueFullTrails = std::vector<FullTrail>;
using Map = std::vector<std::string>;

UniqueTrails trails;
UniqueFullTrails uniqueFullTrails;
Coordinate current_start;
FullTrail current_trail;

void read_input(const std::string &path, Map &input) {
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

bool inBounds(const int &r, const int &c, const Map &map) {
  return (r < map.size() && r >= 0) && (c < map.at(0).size() && c >= 0);
}

void isNextStepAround(const Coordinate &pos, const int &altitude,
                      const Map &map) {
  const std::array<Coordinate, 4> all_dirs = {
      Coordinate(-1, 0), Coordinate(0, -1), Coordinate(0, 1), Coordinate(1, 0)};
  const int r = std::get<0>(pos);
  const int c = std::get<1>(pos);
  if (map.at(r)[c] - 48 == 9 && altitude == 9) {
    trails.insert(Trail(current_start, pos));
    return;
  }

  for (const auto &[dir_r, dir_c] : all_dirs) {
    if (inBounds(r + dir_r, c + dir_c, map)) {
      if ((map.at(r + dir_r)[c + dir_c] - 48) - (map.at(r)[c] - 48) == 1) {
        isNextStepAround(std::pair<int, int>(r + dir_r, c + dir_c),
                         altitude + 1, map);
      }
    }
  }
  return;
}

void isNextStepAround2(const Coordinate &pos, const int &altitude,
                       const Map &map) {
  const std::array<Coordinate, 4> all_dirs = {
      Coordinate(-1, 0), Coordinate(0, -1), Coordinate(0, 1), Coordinate(1, 0)};
  const int r = std::get<0>(pos);
  const int c = std::get<1>(pos);

  current_trail.push_back(Coordinate(r, c));
  if (map.at(r)[c] - 48 == 9 && altitude == 9) {
    uniqueFullTrails.push_back(current_trail);
    return;
  }

  for (const auto &[dir_r, dir_c] : all_dirs) {
    if (inBounds(r + dir_r, c + dir_c, map)) {
      if ((map.at(r + dir_r)[c + dir_c] - 48) - (map.at(r)[c] - 48) == 1) {
        isNextStepAround2(std::pair<int, int>(r + dir_r, c + dir_c),
                          altitude + 1, map);
      }
    }
  }
  return;
}

int getTrailCount(const Map &map) {
  for (int r = 0; r < map.size(); r++) {
    for (int c = 0; c < map.at(0).size(); c++) {
      if (map.at(r)[c] - 48 == 0) {
        current_start = std::pair<int, int>(r, c);
        isNextStepAround(std::pair<int, int>(r, c), 0, map);
      }
    }
  }

  for (auto it : trails) {
    std::cout << "Start =" << std::get<0>(std::get<0>(it)) << ":"
              << std::get<1>(std::get<0>(it))
              << " End=" << std::get<0>(std::get<1>(it)) << ":"
              << std::get<1>(std::get<1>(it)) << std::endl;
  }
  return trails.size();
}

int getTrailCount2(const Map &map) {
  for (int r = 0; r < map.size(); r++) {
    for (int c = 0; c < map.at(0).size(); c++) {
      if (map.at(r)[c] - 48 == 0) {
        current_trail = FullTrail();
        isNextStepAround2(Coordinate(r, c), 0, map);
      }
    }
  }

  for (auto trail : uniqueFullTrails) {
    for (auto pos : trail) {
      std::cout << " " << std::get<0>(pos) << ":" << std::get<1>(pos);
    }
    std::cout << std::endl;
  }
  return uniqueFullTrails.size();
}

int main(int argc, char *argv[]) {
  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }

  Map example;
  read_input(infile, example);

  if (step == "1") {
    auto tc = getTrailCount(example);
    std::cout << "result = " << tc << std::endl;
  }

  if (step == "2") {
    auto tc = getTrailCount2(example);
    std::cout << "result = " << tc << std::endl;
  }
  return 0;
}
