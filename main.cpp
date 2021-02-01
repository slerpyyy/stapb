#include <chrono>
#include <thread>

#include "stapb.hpp"

void compute() {
    int total = 1000;

    PROG_INIT(total);

    #pragma omp parallel for
    for (int i = 0; i < total; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        PROG_INC;
    }
}

int main() {
    compute();
    return 0;
}
