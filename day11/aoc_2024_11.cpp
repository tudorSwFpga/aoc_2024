#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

int nbDigits(long long i) {
  int digits = 0;
  while (i > 0) {
    i /= 10;
    digits++;
  }
  return digits;
}

void getDigits(const long long &i, long long &left, long long &right) {
  long tmp = i;
  for (int j = 0; j < nbDigits(i) / 2; j++) {
    tmp /= 10;
  }
  left = tmp;
  right = i - tmp * pow(10, nbDigits(i) / 2);
}

int algo(std::vector<long long> &stones, const int &steps) {
  for (int i = 0; i < steps; ++i) {
    for (auto it = stones.begin(); it < stones.end(); it++) {
      if (*it == 0) {
        it = stones.erase(it);
        it = stones.insert(it, 1);
      } else if (nbDigits(*it) % 2 == 0) {
        long long left, right;
        getDigits(*it, left, right);
        it = stones.erase(it);
        it = stones.insert(it, left);
        it = stones.insert(it + 1, right);
      } else {
        const long val = *it * 2024;
        it = stones.erase(it);
        it = stones.insert(it, val);
      }
    }
    std::cout << "Step " << i << " count = " << stones.size() << std::endl;
  }
  return stones.size();
}

long long process_stone(const long long &stone, const int &depth,
                        std::map<std::pair<long long, int>, long long> &mmap) {
  std::cout << stone << " depth = " << depth << std::endl;
  if (depth == 75) {
    return 1;
  }

  if (mmap.count(std::pair<long long, int>(stone, depth)) != 0) {
    return mmap[std::pair<long long, int>(stone, depth)];
  }

  if (stone == 0) {
    return process_stone(1, depth + 1, mmap);
  }

  if (nbDigits(stone) % 2 == 0) {
    long long left, right;
    getDigits(stone, left, right);
    const long long result = process_stone(left, depth + 1, mmap) +
                             process_stone(right, depth + 1, mmap);
    mmap[std::pair<long long, int>(stone, depth)] = result;
    return result;
  }

  return process_stone(stone * 2024, depth + 1, mmap);
}

int main(int argc, char *argv[]) {

  std::vector<long long> example = {112,     1110,  163902, 0,
                                    7656027, 83039, 9,      74};
  std::map<std::pair<long long, int>, long long> mmap;
  long long result = 0;
  for (auto stone : example) {
    result += process_stone(stone, 0, mmap);
  }
  std::cout << "Result " << result << std::endl;

  return 0;
}
