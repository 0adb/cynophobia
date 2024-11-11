#include <cynophobia/cynolexer.hpp>

#include <fmt/format.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>

int main() {
    std::vector<double> input = {1.2, 2.3, 3.4, 4.5};

    auto [mean, moment] = accumulate_vector(input);

    fmt::print("Mean: {}, Moment: {}\n",  mean, moment);

    std::system("ls -l > test.txt"); // executes the UNIX command "ls -l >test.txt"
    std::cout << std::ifstream("test.txt").rdbuf();
    std::system("echo 'int main() { return 0; }'> test.txt");
    std::system("gcc -P -E test.txt -o test.i");
    fmt::print("preprocess output:\n");
    std::cout << std::ifstream("test.i").rdbuf();
    std::system("rm test.txt; rm test.i");
    return 0;
}
