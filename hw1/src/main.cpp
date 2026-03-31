#include <iostream>
#include <vector>

#include "Task.hpp"

Task dfs(const std::vector<std::vector<int>>& graph, int node,
         std::vector<bool>& visited) {
    if (visited[node]) {
        co_return;
    }

    visited[node] = true;
    co_yield node;

    for (int to : graph[node]) {
        if (!visited[to]) {
            Task sub = dfs(graph, to, visited);

            while (!sub.Done()) {
                auto v = sub.Next();

                if (!sub.Done()) {
                    co_yield v;
                }
            }
        }
    }
}

int main() {
    std::vector<std::vector<int>> graph = {{1, 2}, {3, 4}, {5}, {}, {}, {}};

    std::vector<bool> visited1(graph.size(), false);
    std::vector<bool> visited2(graph.size(), false);

    Task dfs1 = dfs(graph, 0, visited1);
    Task dfs2 = dfs(graph, 2, visited2);

    while (!dfs1.Done() || !dfs2.Done()) {
        if (!dfs1.Done()) {
            auto v = dfs1.Next();
            if (!dfs1.Done()) {
                std::cout << "DFS1: " << v << std::endl;
            }
        }

        if (!dfs2.Done()) {
            auto v = dfs2.Next();
            if (!dfs2.Done()) {
                std::cout << "DFS2: " << v << std::endl;
            }
        }
    }

    return 0;
}
