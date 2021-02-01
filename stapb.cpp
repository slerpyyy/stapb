#include "stapb.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <cmath>

std::mutex prog_bar_guard;

ProgressBar::ProgressBar(std::string name, uint64_t total) :
    m_name(name), m_curr(0), m_next(1), m_total(total),
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
    prog_bar_guard.lock();
    m_curr = current;
    update();
    prog_bar_guard.unlock();
}

void ProgressBar::add(uint64_t amount) {
    prog_bar_guard.lock();
    m_curr += amount;
    update();
    prog_bar_guard.unlock();
}

void ProgressBar::inc() {
    prog_bar_guard.lock();
    ++m_curr;
    update();
    prog_bar_guard.unlock();
}

void ProgressBar::update(bool force_update) {
    if (!force_update && m_curr < m_next) return;

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - m_start;
    const double f_elapsed = duration.count();

    const double f_total = m_total;
    const double f_prog = m_curr / f_total;
    const double f_left = (f_elapsed / f_prog) - f_elapsed;
    const uint64_t u_prog = round((1 << 5) * f_prog);

    std::stringstream buffer;
    buffer << "\r" << m_name << " [";

    uint64_t i = 0;
    for (; i < (u_prog); ++i) buffer << "#";
    for (; i < (1 << 5); ++i) buffer << "-";

    buffer << std::fixed << std::setprecision(2)
        << "] " << 100.0f * f_prog
        << "%, rate: " << (m_total / f_elapsed) / 1000.0f
        << "k#/s elapsed: " << f_elapsed
        << "s, left: " << f_left << "s ";

    std::cout << buffer.str() << std::flush;

    m_next = m_curr + (m_total >> 6) + 1;
}
