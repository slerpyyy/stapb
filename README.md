# stapb
*simple thread-safe ascii-only progress bar*

A simple ascii-only progress bar, which can safely be used in concurrent code.
The progress bar is eagerly reprinted on every call unless another thread is
already in the process of refreshing it. This implementation is lock-free.

## Usage

You can interface with the progress bar using the following macros:
```cpp
PROG_INIT(N);  // start progress bar with total of N
PROG_SET(N);   // override current progress bar with N
PROG_ADD(N);   // add N to the progress bar
PROG_INC;      // increment progress bar by 1
```
...or by using the `ProgressBar` class directly:
```cpp
class ProgressBar {
public:
    ProgressBar(std::string name, uint64_t total);
    ~ProgressBar();

    void set(uint64_t current);
    void add(uint64_t amount);
    void inc();
};
```

## Example
```cpp
void compute() {
    int total = 1000;

    PROG_INIT(total);

    #pragma omp parallel for
    for (int i = 0; i < total; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        PROG_INC;
    }
}
```
This produces the following output:
```
compute [#######################---------] 70.50%, rate: 0.10k#/s elapsed: 9.68s, left: 4.05s
   ^                   ^                     ^            ^                 ^            ^
   1                   2                     3            4                 5            6
```

1) The name of the function (as taken from `__func__`)
2) A visual loading bar
3) The current percentage
4) The rate of change (in 1000 per second)
5) The time since the progress bar was started (in seconds)
6) The approximate time remaining (in seconds)
