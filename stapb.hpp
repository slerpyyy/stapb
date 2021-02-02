#ifndef STAPB_HPP
#define STAPB_HPP

#include <string>
#include <chrono>
#include <atomic>

#define PROG_INIT(N) ProgressBar _progress_bar(__func__, (uint64_t)(N))
#define PROG_SET(N)  _progress_bar.set((N))
#define PROG_ADD(N)  _progress_bar.add((N))
#define PROG_INC     _progress_bar.inc()

class ProgressBar {
public:
    ProgressBar(std::string name, uint64_t total);
    ~ProgressBar();

    void set(uint64_t current);
    void add(uint64_t amount);
    void inc();

private:
    void update(bool force_update = false);

    std::string m_name;
    std::atomic<uint64_t> m_curr;
    uint64_t m_total;
    std::chrono::high_resolution_clock::time_point m_start;
};

#endif
