#include <cynophobia/cynolexer.hpp>

#include <tuple>
#include <vector>
#include <algorithm> 

std::tuple<double, double> accumulate_vector(const std::vector<double>& values) {

    double count = 0;
    double total = 0;
    double total_squares = 0;
    std::for_each(std::begin(values), std::end(values), [&count, &total, &total_squares](const double &value) {
        count += 1;
        total += value;
        total_squares += (value * value); 
    });

    return {total / count, total_squares / count };
}
