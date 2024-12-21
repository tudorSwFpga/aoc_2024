#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

std::vector<std::string> readFile(const std::string &filename) {
  std::ifstream file(filename);
  std::vector<std::string> ret;

  if (!file) {
    std::cerr << "Error: Could not open the file!" << std::endl;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::cout << "xxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
    std::cout << line << std::endl;
    ret.push_back(line);
  }
  return ret;
}

long computeProgram(const std::string &input) {
  std::regex word_regex("mul\\((\\d+),(\\d+)\\)");
  auto begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
  auto end = std::sregex_iterator();
  long res = 0;
  for (std::sregex_iterator i = begin; i != end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    std::cout << "  " << match_str << '\n';
    std::cout << match[1].str() << '\n';
    std::cout << match[2].str() << '\n';
    res += std::stoi(match[1].str()) * std::stoi(match[2].str());
  }

  return res;
}

long computeProgram2(const std::string &input, bool &activated) {
  // search for do() / don't() as well as mul(x,y)
  std::regex word_regex("(do|don't)\\(\\)|mul\\((\\d+),(\\d+)\\)");
  auto begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
  auto end = std::sregex_iterator();
  long res = 0;
  for (std::sregex_iterator i = begin; i != end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    std::cout << "  " << match_str << '\n';
    if ((match_str == "do()" && !activated) ||
        (match_str == "don't()" && activated)) {
      activated = !activated;
    }
    if (activated && match_str != "do()" && match_str != "don't()") {
      std::cout << match[2].str() << '\n';
      std::cout << match[3].str() << '\n';
      res += std::stoi(match[2].str()) * std::stoi(match[3].str());
    }
  }

  return res;
}

int main(int argc, char *argv[]) {
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }
  auto input = readFile("aoc_2024_3.input");
  long res = 0;
  if (step == "1") {
    for (auto &i : input) {
      res += computeProgram(i);
      std::cout << "Res is now " << res << std::endl;
    }
  }

  if (step == "2") {
    bool activated = true;
    for (auto &i : input) {
      res += computeProgram2(i, activated);
      std::cout << "Res is now " << res << std::endl;
    }
  }

  return 0;
}
