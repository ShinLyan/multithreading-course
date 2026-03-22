#include "Scheduler.hpp"

void Scheduler::AddTask(Task task) { new_tasks_.push_back(std::move(task)); }

void Scheduler::Run() {
    tasks_.insert(tasks_.end(), std::make_move_iterator(new_tasks_.begin()),
                  std::make_move_iterator(new_tasks_.end()));
    new_tasks_.clear();

    while (!tasks_.empty()) {
        Task task = std::move(tasks_.back());
        tasks_.pop_back();

        bool still_running = task.Run(*this);

        if (!new_tasks_.empty()) {
            tasks_.insert(tasks_.end(),
                          std::make_move_iterator(new_tasks_.begin()),
                          std::make_move_iterator(new_tasks_.end()));
            new_tasks_.clear();
        }

        if (still_running) {
            tasks_.push_back(std::move(task));
        }
    }
}