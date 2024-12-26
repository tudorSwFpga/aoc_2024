#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

// Function to perform topological sorting
std::vector<int>
topologicalSort(const std::set<int> &nodes,
                const std::vector<std::pair<int, int>> &edges) {
  // Adjacency list and in-degree map
  std::unordered_map<int, std::vector<int>> adj;
  std::unordered_map<int, int> in_degree;

  // Initialize in-degree for all nodes
  for (int node : nodes) {
    in_degree[node] = 0; // Ensure all nodes are accounted for
  }

  // Build the adjacency list and update in-degree
  for (const auto &edge : edges) {
    adj[edge.first].push_back(edge.second);
    in_degree[edge.second]++;
  }

  // Queue for nodes with in-degree 0
  std::queue<int> q;
  for (const auto &entry : in_degree) {
    if (entry.second == 0) {
      std::cout << "In degree 0 found \n";
      q.push(entry.first);
    }
  }

  // Perform topological sort
  std::vector<int> topo_order;
  while (!q.empty()) {
    std::cout << "popping from q\n";
    int curr = q.front();
    q.pop();
    topo_order.push_back(curr);

    for (int neighbor : adj[curr]) {
      in_degree[neighbor]--;
      if (in_degree[neighbor] == 0) {
        q.push(neighbor);
      }
    }
  }

  // Check for cycles
  if (topo_order.size() != nodes.size()) {
    std::cout << "topo order " << topo_order.size() << " vs nodes "
              << nodes.size();
    // throw std::runtime_error("Graph contains a cycle. Topological sort not
    // possible.");
  }

  return topo_order;
}

void read_rules(const std::string &path,
                std::vector<std::pair<int, int>> &edges, std::set<int> &nodes) {
  // Read input from file
  std::ifstream infile(path);
  if (!infile) {
    throw std::runtime_error("Failed to open input file.");
  }

  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    int left, right;
    char sep;
    if (iss >> left >> sep >> right && sep == '|') {
      edges.emplace_back(left, right);
      nodes.insert(left);
      nodes.insert(right);
    } else {
      throw std::runtime_error("Invalid input format: " + line);
    }
  }

  infile.close();
}

void read_input(const std::string &path, std::vector<std::vector<int>> &input) {
  // Read input from file
  std::ifstream infile(path);
  if (!infile) {
    throw std::runtime_error("Failed to open input file.");
  }

  std::string line;
  const char sep = ',';
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    std::vector<int> l;
    std::string token;
    while (std::getline(iss, token, ',')) { // Delimit by comma
      l.push_back(std::stoi(token));
    }
    input.push_back(l);
  }

  infile.close();
}

int getMidVal(const std::vector<int> &input) {
  const int mid_pos = (input.size() - 1) / 2;
  return input.at(mid_pos);
}

bool isOrder(const std::vector<int> &rules, std::vector<int> input) {
  std::vector<int> positions;
  std::vector<int> reordered;
  bool isOrderCorrect = true;
  for (const auto &i : input) {
    for (int r = 0; r < rules.size(); r++) {
      if (i == rules.at(r))
        positions.push_back(r);
    }
  }
  // if order is not good then we have an issue
  for (int i = 1; i < positions.size(); i++) {
    if (positions.at(i) <= positions.at(i - 1)) {
      isOrderCorrect = false;
      std::cout << "Echec ici" << std::endl;
    }
  }

  return isOrderCorrect;
}

int main(int argc, char *argv[]) {

  const std::string infile = argv[1];
  std::string step = "1";
  if (argc > 1) {
    step = argv[2];
  }

  std::vector<std::pair<int, int>> edges;
  std::set<int> nodes;

  read_rules(infile + ".rules", edges, nodes);
  std::vector<int> rules = topologicalSort(nodes, edges);
  for (const auto &i : rules) {
    std::cout << i << " ";
  }

  std::vector<std::vector<int>> input;
  read_input(infile, input);

  int val = 0;
  for (const auto &line : input) {
    std::cout << "Parsing new input line" << std::endl;
    if (isOrder(rules, line)) {
      val += getMidVal(line);
    }
  }
  std::cout << "Final val =" << val << std::endl;
}
