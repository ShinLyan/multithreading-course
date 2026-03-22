#pragma once

#include <vector>

#include "Task.hpp"

class Scheduler {
   public:
    void AddTask(Task task);

    void Run();

   private:
    std::vector<Task> tasks_;
    std::vector<Task> new_tasks_;
};
