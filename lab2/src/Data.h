#ifndef DATA_H
#define DATA_H

#include <vector>
#include <cstddef>

class Data{
public:
    std::vector<int> arr;
    int min, max;
    double average;
    Data() {
        min = 0;
        max = 0;
        average = 0;
    }
    Data(const std::vector<int>& _arr) {
        arr.resize(_arr.size());
        for (size_t i = 0; i < _arr.size(); ++i) {
            arr[i] = _arr[i];
        }
        min = max = 0;
        average = 0.0;
    }
};

#endif 