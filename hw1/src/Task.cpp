#include "Task.hpp"

#include "Scheduler.hpp"

Task::Task(Func f) : func_(std::move(f)) {}

bool Task::Run(Scheduler& scheduler) { return func_(scheduler); }