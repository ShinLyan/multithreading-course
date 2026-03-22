#include <iostream>
#include <vector>

#include "Scheduler.hpp"

int main() {
    Scheduler scheduler;

    std::vector<std::vector<int>> graph = {{1, 2}, {3}, {}, {}};

    std::vector<bool> visited(graph.size(), false);

    std::function<Task(int)> make_task;

    make_task = [&](int v) -> Task {
        return Task([&, v](Scheduler& sched) {
            if (visited[v]) {
                return false;
            }

            visited[v] = true;
            std::cout << "Visit " << v << std::endl;

            for (auto it = graph[v].rbegin(); it != graph[v].rend(); ++it) {
                sched.AddTask(make_task(*it));
            }

            return false;
        });
    };

    for (int v = 0; v < (int)graph.size(); ++v) {
        if (!visited[v]) {
            scheduler.AddTask(make_task(v));
        }
    }

    scheduler.Run();

    return 0;
}
