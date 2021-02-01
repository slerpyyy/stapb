# stapb
*simple thread-safe ascii-only progress bar*

## Usage

You can interface with the progress bar using the following macros:
```cpp
PROG_INIT(N)  // start progress bar with total of N
PROG_SET(N)   // override current progress bar with N
PROG_ADD(N)   // add N to the progress bar
PROG_INC      // increment progress bar by 1
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
        auto duration = std::chrono::milliseconds(100);
        std::this_thread::sleep_for(duration);

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

1) name of the function
2) loading bar
3) percentage
4) rate of change (in 1000 per second)
5) the time since the progress bar was started
6) approximate time remaining
