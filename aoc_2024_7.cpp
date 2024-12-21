#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
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

class ThreadPool {
public:
  ThreadPool(const uint32_t &nproc) : m_nproc(nproc) {
    spdlog::debug("ThreadPool ctor with {} threads", nproc);
  };
  void start();
  void QueueJob(const Func &job, const FuncArgs &args);
  void stop();

  long long getResult() { return result; }

  void getSuccessfulEquations(std::vector<FuncArgs> &l) {
    l = successful_equations;
  }

  bool isBusy() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return jobs.size() != 0;
  }

private:
  void ThreadLoop();
  uint32_t m_nproc;
  bool should_terminate = false; // Tells threads to stop looking for jobs
  std::mutex queue_mutex;        // Prevents data races to the job queue
  std::mutex result_mutex;       // Prevents data races to the job queue
  std::condition_variable
      mutex_condition; // Allows threads to wait on new jobs or termination
  std::vector<std::thread> threads;
  std::queue<std::pair<Func, FuncArgs>> jobs;
  long long result = 0;
  std::vector<FuncArgs> successful_equations;
};

void ThreadPool::start() {
  threads.resize(m_nproc);
  std::cout << "Starting N threads\n";
  for (uint32_t i = 0; i < m_nproc; i++) {
    threads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
  }
}

void ThreadPool::ThreadLoop() {
  while (true) {
    std::pair<Func, FuncArgs> job;
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      mutex_condition.wait(
          lock, [this] { return !jobs.empty() || should_terminate; });
      if (should_terminate) {
        return;
      }
      job = jobs.front();
      jobs.pop();
    }
    if (job.first(job.second) == true) {
      std::unique_lock<std::mutex> lock(result_mutex);
      std::cout << "Equation " << std::get<1>(job.second) << std::endl;
      successful_equations.push_back(job.second);
      result += std::get<1>(job.second);
    }
  }
}

void ThreadPool::QueueJob(const Func &job, const FuncArgs &args) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    jobs.push(std::make_pair(job, args));
  }
  mutex_condition.notify_one();
}

void ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    should_terminate = true;
  }
  mutex_condition.notify_all();
  for (std::thread &active_thread : threads) {
    active_thread.join();
  }
  threads.clear();
}

std::pair<std::vector<long long>, long long>
parse_line(const std::string &line) {
  std::stringstream ss(line);
  std::string token;

  // Get the integer before ':'
  std::getline(ss, token, ':');
  long long numberBeforeColon = std::stoll(token);

  // Get the vector of integers after ':'
  std::vector<long long> numbersAfterColon;
  while (std::getline(ss, token, ' ')) {
    if (token != "") {
      numbersAfterColon.push_back(std::stoll(token));
    }
  }

  return {numbersAfterColon, numberBeforeColon};
}

void read_input(const std::string &path, std::vector<FuncArgs> &data) {
  std::ifstream inputFile(path);
  if (!inputFile) {
    throw std::runtime_error("Could not open file " + path);
  }
  std::string line;
  // Read each line from the file
  while (std::getline(inputFile, line)) {
    if (!line.empty()) {
      data.push_back(parse_line(line));
    }
  }

  inputFile.close();
}

bool is_operation_ok(const FuncArgs &input) {
  const int numbers_count = std::get<0>(input).size();
  for (int operator_combination = 0;
       operator_combination < pow(2, numbers_count - 1);
       operator_combination++) {
    // each bit in i will tell us whether we should put a +(for 0) or a *(for 1)
    std::cout << "Operator combination " << operator_combination << std::endl;
    long long tmp = 0;
    int operator_index = 0;
    bool first_done = false;
    for (int operator_index = 0; operator_index < numbers_count;
         operator_index++) {
      if (first_done) {
        if ((operator_combination &
             static_cast<int>(pow(2, operator_index - 1))) == 0) {
          std::cout << tmp << " + " << std::get<0>(input).at(operator_index)
                    << std::endl;
          tmp = tmp + std::get<0>(input).at(operator_index);
        } else {
          std::cout << tmp << " * " << std::get<0>(input).at(operator_index)
                    << std::endl;
          tmp = tmp * std::get<0>(input).at(operator_index);
        }
      } else {
        tmp = std::get<0>(input).at(operator_index);
        first_done = true;
      }
    }
    if (tmp == std::get<1>(input)) {
      return true;
    }
  }
  return false;
}

double nbDigits(long long i) {
  double digits = 0.0;
  while (i > 0) {
    i /= 10;
    digits++;
  }
  return digits;
}

long long concat(const long long &a, const long long &b) {
  return a * static_cast<long long>(pow(10.0, nbDigits(b))) + b;
}

bool is_operation_ok_step2(const FuncArgs &input) {
  const int numbers_count = std::get<0>(input).size();
  for (int operator_combination = 0;
       operator_combination < pow(3, numbers_count - 1);
       operator_combination++) {
    std::cout << "Operator combination " << operator_combination << std::endl;
    long long tmp = 0;
    int operator_index = 0;
    bool first_done = false;
    int value = operator_combination;
    for (int operator_index = 0; operator_index < numbers_count;
         operator_index++) {
      if (first_done) {
        if ((value % 3 == 0)) {
          std::cout << tmp << " + " << std::get<0>(input).at(operator_index)
                    << std::endl;
          tmp = tmp + std::get<0>(input).at(operator_index);
        } else if (value % 3 == 1) {
          std::cout << tmp << " * " << std::get<0>(input).at(operator_index)
                    << std::endl;
          tmp = tmp * std::get<0>(input).at(operator_index);
        } else {
          std::cout << tmp << " || " << std::get<0>(input).at(operator_index)
                    << std::endl;
          tmp = concat(tmp, std::get<0>(input).at(operator_index));
        }
        value /= 3;
      } else {
        tmp = std::get<0>(input).at(operator_index);
        first_done = true;
      }
    }
    if (tmp == std::get<1>(input)) {
      std::cout << "Found OK for" << std::endl;
      return true;
    }
  }
  return false;
}

int main(int argc, char *argv[]) {
  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }

  std::vector<FuncArgs> input;
  read_input("aoc_2024_7.input", input);
  std::vector<FuncArgs> successful_equations;
  std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>(12);
  tp->start();

  for (const auto &operation : input) {
    tp->QueueJob(is_operation_ok, operation);
  }

  sleep(30);
  std::cout << "Result = " << tp->getResult() << std::endl;
  tp->getSuccessfulEquations(successful_equations);

  std::cout << "************STEP2" << std::endl;

  for (const auto &operation : input) {
    // if alredy successful during step 1, skip it
    bool found = false;
    for (const auto &equation : successful_equations) {
      if (equation == operation)
        found = true;
    }
    if (!found) {
      tp->QueueJob(is_operation_ok_step2, operation);
    }
  }

  sleep(200);
  tp->stop();
  std::cout << "Result = " << tp->getResult() << std::endl;
}
