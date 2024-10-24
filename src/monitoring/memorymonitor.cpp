// src/MemoryMonitor.cpp
#include "memorymonitor.h"
#include <spdlog/spdlog.h>
#include <unistd.h>

#if defined(__linux__)
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#endif

MemoryMonitor::MemoryMonitor(double threshold, std::chrono::seconds interval)
    : threshold_(threshold), interval_(interval), running_(false) {}

MemoryMonitor::~MemoryMonitor() {
    stop();
}

void MemoryMonitor::start() {
    if (running_) {
        return;
    }
    running_ = true;
    monitor_thread_ = std::thread(&MemoryMonitor::monitor, this);
}

void MemoryMonitor::stop() {
    if (!running_) {
        return;
    }
    running_ = false;
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
}

void MemoryMonitor::monitor() {
    while (running_) {
        double usage = 0.0;

#if defined(__linux__)
        struct sysinfo memInfo;
        if (sysinfo(&memInfo) == 0) {
            long long totalPhysMem = memInfo.totalram;
            totalPhysMem *= memInfo.mem_unit;

            long long physMemUsed = memInfo.totalram - memInfo.freeram;
            physMemUsed *= memInfo.mem_unit;

            usage = (double)(physMemUsed) / (double)(totalPhysMem) * 100.0;
        } else {
            spdlog::error("Failed to get memory info using sysinfo.");
        }
#elif defined(__APPLE__)
        mach_port_t mach_port = mach_host_self();
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        vm_statistics64_data_t vm_stats;

        if (host_statistics64(mach_port, HOST_VM_INFO, reinterpret_cast<host_info64_t>(&vm_stats), &count) == KERN_SUCCESS) {
            int64_t free_memory = vm_stats.free_count * sysconf(_SC_PAGESIZE);
            int64_t active_memory = vm_stats.active_count * sysconf(_SC_PAGESIZE);
            int64_t inactive_memory = vm_stats.inactive_count * sysconf(_SC_PAGESIZE);
            int64_t speculative_memory = vm_stats.speculative_count * sysconf(_SC_PAGESIZE);
            int64_t wired_memory = vm_stats.wire_count * sysconf(_SC_PAGESIZE);
            int64_t compressed_memory = vm_stats.compressor_page_count * sysconf(_SC_PAGESIZE);

            int64_t total_memory = free_memory + active_memory + inactive_memory + speculative_memory + wired_memory + compressed_memory;
            int64_t used_memory = active_memory + inactive_memory + speculative_memory + wired_memory + compressed_memory;

            usage = (double)(used_memory) / (double)(total_memory) * 100.0;
        } else {
            spdlog::error("Failed to get memory info using mach_host_self.");
        }
#endif

        if (usage > threshold_) {
            spdlog::warn("Memory usage high: {:.2f}% exceeds threshold of {:.2f}%", usage, threshold_);
        } else {
            spdlog::debug("Memory usage: {:.2f}%.", usage);
        }

        std::this_thread::sleep_for(interval_);
    }
}
