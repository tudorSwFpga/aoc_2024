#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

using Coordinate = std::pair<int, int>;
// trail = a start and an end
using Area = std::vector<Coordinate>;
using AreaList = std::map<char, std::vector<Area>>;
using Map = std::vector<std::string>;

AreaList area_list;
Area curr_area;

std::set<Coordinate> visited;

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

int diff_r(const Coordinate &a, const Coordinate b) {
  return std::get<0>(a) - std::get<0>(b);
}

int diff_c(const Coordinate &a, const Coordinate b) {
  return std::get<1>(a) - std::get<1>(b);
}

bool inBounds(const int &r, const int &c, const Map &map) {
  return (r < map.size() && r >= 0) && (c < map.at(0).size() && c >= 0);
}

void isVeggieAround(const char &v, const Coordinate &pos, const Map &map) {
  const std::array<Coordinate, 8> all_dirs = {
      Coordinate(-1, -1), Coordinate(-1, 0), Coordinate(-1, 1),
      Coordinate(0, 1),   Coordinate(1, 1),  Coordinate(1, 0),
      Coordinate(1, -1),  Coordinate(0, -1)};
  const int r = std::get<0>(pos);
  const int c = std::get<1>(pos);

  curr_area.push_back(pos);
  visited.insert(pos);
  for (const auto &[dir_r, dir_c] : all_dirs) {
    const auto next_pos = Coordinate(r + dir_r, c + dir_c);
    if (inBounds(r + dir_r, c + dir_c, map)) {
      if ((map.at(r + dir_r)[c + dir_c]) == v && visited.count(next_pos) == 0) {
        std::cout << "isVeggieAround " << r + dir_r << " : " << c + dir_c
                  << std::endl;
        isVeggieAround(v, std::pair<int, int>(r + dir_r, c + dir_c), map);
      }
    }
  }
}

void getVeggies(const Map &map) {
  for (int r = 0; r < map.size(); r++) {
    for (int c = 0; c < map.at(0).size(); c++) {
      const char veggie = map.at(r)[c];
      if (visited.count(Coordinate(r, c)) == 0) {
        curr_area = Area();
        std::cout << "Checking at " << r << " : " << c << std::endl;
        isVeggieAround(veggie, Coordinate(r, c), map);
        std::cout << "push_back" << std::endl;
        area_list[veggie].push_back(curr_area);
      }
    }
  }
}

int getArea(const Area &veg_list) { return veg_list.size(); }

int getPerimeter(const Area &veg_list) {
  int p = veg_list.size() * 4;
  std::set<std::pair<Coordinate, Coordinate>> common_sides;
  for (int i = 0; i < veg_list.size(); i++) {
    for (int j = 0; j < veg_list.size(); j++) {
      if (veg_list.at(i) == veg_list.at(j)) {
        continue;
      }
      if ((((abs(diff_r(veg_list.at(i), veg_list.at(j))) == 1) &&
            diff_c(veg_list.at(i), veg_list.at(j)) == 0) ||
           ((diff_r(veg_list.at(i), veg_list.at(j)) == 0) &&
            abs(diff_c(veg_list.at(i), veg_list.at(j))) == 1))) {
        --p;
      }
    }
  }
  return p;
}

int main(int argc, char *argv[]) {
  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }

  Map example;
  read_input(infile, example);
  getVeggies(example);

  long price = 0;

  for (const auto &[key, value] : area_list) {
    std::cout << key << " at " << std::endl;
    for (auto v : value) {
      for (auto it : v) {
        std::cout << std::get<0>(it) << ":" << std::get<1>(it) << std::endl;
      }
      price += getPerimeter(v) * getArea(v);
      std::cout << "Perimeter = " << getPerimeter(v) << std::endl;
    }
  }
  std::cout << "Result = " << price << std::endl;

  return 0;
}
