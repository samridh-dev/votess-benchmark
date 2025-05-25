#ifndef GENERATE_UNIFORM_DATASET_HPP
#define GENERATE_UNIFORM_DATASET_HPP

#include <random>

template <typename iter, typename T>
void
generate_uniform_dataset(iter it, T n) 
{

        using F = typename std::iterator_traits<iter>::value_type;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<F> dist(0.0, 1.0);
        for (T i{0}; i < n; ++i) {
                *(it + i) = dist(gen);
        }

}

#endif // GENERATE_UNIFORM_DATASET_HPP
