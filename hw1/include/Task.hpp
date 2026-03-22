#pragma once

#include <functional>

class Scheduler;

class Task {
   public:
    using Func = std::function<bool(Scheduler&)>;

    explicit Task(Func f);

    bool Run(Scheduler& scheduler);

   private:
    Func func_;
};
