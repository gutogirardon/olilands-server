// src/MemoryMonitor.h
#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <thread>
#include <atomic>
#include <chrono>

class MemoryMonitor {
public:
    MemoryMonitor(double threshold = 80.0, std::chrono::seconds interval = std::chrono::seconds(5));
    ~MemoryMonitor();

    void start();
    void stop();

private:
    void monitor();

    double threshold_; // Threshold percentage
    std::chrono::seconds interval_; // Interval between checks
    std::thread monitor_thread_;
    std::atomic<bool> running_;
};

#endif // MEMORY_MONITOR_H
