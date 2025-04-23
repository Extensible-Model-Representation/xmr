#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
namespace XMR {

// Utility class to help model dependencies and allow return topilogical sort
class DependencyGraph {
 public:
  void addEdge(std::string v, std::string w) { adj[v].push_back(w); }

  std::vector<std::string> topSort() {
    std::vector<std::string> sorted;
    std::stack<std::string> stack;
    std::unordered_map<std::string, bool> visited;

    for (auto& v : adj) {
      visited[v.first] = false;
    }

    for (auto& v : adj) {
      if (visited[v.first] == false) topSortUtil(v.first, visited, stack);
    }

    while (!stack.empty()) {
      sorted.push_back(stack.top());
      stack.pop();
    }
    return sorted;
  }

 private:
  std::unordered_map<std::string, std::list<std::string>> adj;
  void topSortUtil(std::string v, std::unordered_map<std::string, bool>& visited, std::stack<std::string>& stack) {
    std::cout << "Visiting node: " << v << std::endl;
    visited[v] = true;
    std::list<std::string>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i) {
      if (!visited[*i]) {
        topSortUtil(*i, visited, stack);
      }
    }
    stack.push(v);
  }
};
}  // namespace XMR