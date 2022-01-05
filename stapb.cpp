#include "stapb.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <cmath>

std::mutex prog_bar_guard;

ProgressBar::ProgressBar(std::string name, uint64_t total) :
    m_name(name), m_curr(0), m_total(total),
    m_start(std::chrono::high_resolution_clock::now())
{}

ProgressBar::~ProgressBar() {
    update(true);
    std::cout << "DONE!\n";

    if (m_curr != m_total)
        std::cout << "\twarning: counter stopped at "
            << m_curr << "/" << m_total << "\n";

    std::cout << std::flush;
}

void ProgressBar::set(uint64_t current) {
    m_curr.store(current, std::memory_order_release);
    update();
}

void ProgressBar::add(uint64_t amount) {
    m_curr.fetch_add(amount, std::memory_order_acq_rel);
    update();
}

void ProgressBar::inc() {
    m_curr.fetch_add(1, std::memory_order_acq_rel);
    update();
}

void ProgressBar::update(bool force_update) {
    if (force_update) {
        prog_bar_guard.lock();
    } else if (
        !prog_bar_guard.try_lock()
    ) return;

    const uint64_t curr = m_curr.load(std::memory_order_acquire);

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - m_start;
    const double f_elapsed = duration.count();

    const double f_total = m_total;
    const double f_prog = curr / f_total;
    const double f_left = (f_elapsed / f_prog) - f_elapsed;

    constexpr uint64_t bar_width = 1 << 5;
    const bool over_limit = f_prog > 1.0;
    const uint64_t u_prog = over_limit ? bar_width : round(bar_width * f_prog);

    uint64_t i = 0;
    char prog_bar[bar_width + 1];
    for (; i < u_prog;    ++i) prog_bar[i] = '#';
    for (; i < bar_width; ++i) prog_bar[i] = '-';
    prog_bar[bar_width] = 0;

    std::printf(
        "\r%s [%s]%s %.2f%, rate: %.2fk#/s elapsed: %.2fs, left: %.2fs ",
        m_name.c_str(), prog_bar, "?" + (!over_limit), 100.0f * f_prog,
        (m_total / f_elapsed) / 1000.0f, f_elapsed, f_left
    );

    std::cout << std::flush;

    prog_bar_guard.unlock();
}
