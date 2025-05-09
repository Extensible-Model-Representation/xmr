/**********************************************************
 * Copyright 2025 Jason Cisneros & Lucas Van Der Heijden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @filename: Graph.hpp
 * @brief:
 *
 ***********************************************************/
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

    // init visited map
    for (auto& v : adj) {
      visited[v.first] = false;
    }

    // Go down each node and visit it's children recursively, if
    // already visited do not need to go down the adj list again
    for (auto& v : adj) {
      if (visited[v.first] == false) topSortUtil(v.first, visited, stack);
    }

    // Pop the sorted vertices
    while (!stack.empty()) {
      sorted.push_back(stack.top());
      stack.pop();
    }
    return sorted;
  }

  // Check if graph has cycle
  bool hasCycle() {
    if (adj.size() <= 1) return false;

    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, bool> recStack;

    // init visited map
    for (auto& v : adj) {
      visited[v.first] = false;
    }

    // init visited map
    for (auto& v : adj) {
      recStack[v.first] = false;
    }

    // Check for cycles starting from every unvisited node
    for (auto& v : adj) {
      if (!visited[v.first] && isCyclicUtil(v.first, visited, recStack)) return true;  // Cycle found
    }

    return false;  // No cycles detected
  }

 private:
  std::unordered_map<std::string, std::list<std::string>> adj;

  // DFS utility to do topological sort
  void topSortUtil(std::string v, std::unordered_map<std::string, bool>& visited, std::stack<std::string>& stack) {
    // mark current node as vistited
    visited[v] = true;
    std::list<std::string>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i) {
      // Go down each adjacent edge and visit until you reach a leaf, marking each as visited as you pass threw
      if (!visited[*i]) {
        topSortUtil(*i, visited, stack);
      }
    }

    // push current node on the stack after visiting all children nodes
    stack.push(v);
  }

  // Utility function for DFS to detect a cycle in a directed graph
  bool isCyclicUtil(std::string u, std::unordered_map<std::string, bool>& visited, std::unordered_map<std::string, bool>& recStack) {
    // If the node is already in the recursion stack, a cycle is detected
    if (recStack[u]) return true;

    // If the node is already visited and not in recursion stack, no need to check again
    if (visited[u]) return false;

    // Mark the current node as visited and add it to the recursion stack
    visited[u] = true;
    recStack[u] = true;

    // Recur for all neighbors
    for (auto& x : adj[u]) {
      if (isCyclicUtil(x, visited, recStack)) {
        std::cerr << "Found cycle at node: " << x << std::endl;
        return true;
      }
    }

    // Remove the node from the recursion stack
    recStack[u] = false;
    return false;
  }
};
}  // namespace XMR