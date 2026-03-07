#include <iostream>
#include "Threads.h"
using namespace std;

DWORD WINAPI min_max(LPVOID ArrData)
{
    cout << "[min_max] Thread started" << endl;
    Data* data = static_cast<Data*>(ArrData);

    if (data->arr.empty()) {
        cout << "[min_max] Empty array\n";
        return 1;
    }

    data->min = 0;
    data->max = 0;
    cout << "[min_max] Initial min=" << data->arr[0] << ", max=" << data->arr[0] << endl;

    for (size_t i = 1; i < data->arr.size(); i++) {
        if (data->arr[i] < data->arr[data->min]) {
            data->min = i;
            cout << "[min_max] New min at index " << i << ": " << data->arr[i] << endl;
        }
        Sleep(7);

        if (data->arr[i] > data->arr[data->max]) {
            data->max = i;
            cout << "[min_max] New max at index " << i << ": " << data->arr[i] << endl;
        }
        Sleep(7);
    }

    cout << "[min_max] FINAL: min=" << data->arr[data->min] << " (index " << data->min
        << "), max=" << data->arr[data->max] << " (index " << data->max << ")" << endl;

    return 0;
}

DWORD WINAPI average(LPVOID ArrData)
{
    cout << "[average] Thread started" << endl;
    Data* data = static_cast<Data*>(ArrData);

    if (data->arr.size() == 0) {
        cout << "[average] Empty array\n";
        return 1;
    }

    long long sum = 0;
    for (size_t i = 0; i < data->arr.size(); i++) {
        sum += data->arr[i];
        cout << "[average] Sum after element " << i << " (" << data->arr[i]
            << "): " << sum << endl;
        Sleep(12);
    }

    data->average = static_cast<double>(sum) / data->arr.size();
    cout << "[average] FINAL: sum=" << sum << ", average=" << data->average << endl;

    return 0;
}