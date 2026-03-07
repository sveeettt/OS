#include "../src/Threads.h"
#include "../src/Data.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <Windows.h>

using namespace std;

void test_min_max_avg_with_positive_numbers() {
    cout << "\n=== Test: Positive numbers ===" << endl;
    vector<int> arr = { 10, 20, 30, 40, 50 };
    Data* data = new Data(arr);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, (void*)data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, (void*)data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "ERROR: Failed to create threads" << endl;
        if (hMinMax) CloseHandle(hMinMax);
        if (hAverage) CloseHandle(hAverage);
        delete data;
        return;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    assert(data->arr[data->min] == 10);
    assert(data->arr[data->max] == 50);
    assert(data->average == 30.0);

    cout << "Min: " << data->arr[data->min] << " (expected 10)" << endl;
    cout << "Max: " << data->arr[data->max] << " (expected 50)" << endl;
    cout << "Average: " << data->average << " (expected 30)" << endl;
    cout << "Test PASSED\n";

    delete data;
}

void test_min_max_avg_with_negative_numbers() {
    cout << "\n=== Test: Negative numbers ===" << endl;
    vector<int> arr = { -10, -5, 0, 5, 10 };
    Data* data = new Data(arr);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, (void*)data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, (void*)data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "ERROR: Failed to create threads" << endl;
        if (hMinMax) CloseHandle(hMinMax);
        if (hAverage) CloseHandle(hAverage);
        delete data;
        return;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    assert(data->arr[data->min] == -10);
    assert(data->arr[data->max] == 10);
    assert(data->average == 0.0);

    cout << "Min: " << data->arr[data->min] << " (expected -10)" << endl;
    cout << "Max: " << data->arr[data->max] << " (expected 10)" << endl;
    cout << "Average: " << data->average << " (expected 0)" << endl;
    cout << "Test PASSED\n";

    delete data;
}

void test_min_max_avg_with_identical_numbers() {
    cout << "\n=== Test: Identical numbers ===" << endl;
    vector<int> arr = { 7, 7, 7, 7, 7 };
    Data* data = new Data(arr);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, (void*)data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, (void*)data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "ERROR: Failed to create threads" << endl;
        if (hMinMax) CloseHandle(hMinMax);
        if (hAverage) CloseHandle(hAverage);
        delete data;
        return;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    assert(data->arr[data->min] == 7);
    assert(data->arr[data->max] == 7);
    assert(data->average == 7.0);

    cout << "Min: " << data->arr[data->min] << " (expected 7)" << endl;
    cout << "Max: " << data->arr[data->max] << " (expected 7)" << endl;
    cout << "Average: " << data->average << " (expected 7)" << endl;
    cout << "Test PASSED\n";

    delete data;
}

void test_min_max_avg_with_single_element() {
    cout << "\n=== Test: Single element ===" << endl;
    vector<int> arr = { 42 };
    Data* data = new Data(arr);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, (void*)data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, (void*)data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "ERROR: Failed to create threads" << endl;
        if (hMinMax) CloseHandle(hMinMax);
        if (hAverage) CloseHandle(hAverage);
        delete data;
        return;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    assert(data->arr[data->min] == 42);
    assert(data->arr[data->max] == 42);
    assert(data->average == 42.0);

    cout << "Min: " << data->arr[data->min] << " (expected 42)" << endl;
    cout << "Max: " << data->arr[data->max] << " (expected 42)" << endl;
    cout << "Average: " << data->average << " (expected 42)" << endl;
    cout << "Test PASSED\n";

    delete data;
}

void test_min_max_avg_with_empty_array() {
    cout << "\n=== Test: Empty array ===" << endl;
    vector<int> arr = {};
    Data* data = new Data(arr);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, (void*)data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, (void*)data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "ERROR: Failed to create threads" << endl;
        if (hMinMax) CloseHandle(hMinMax);
        if (hAverage) CloseHandle(hAverage);
        delete data;
        return;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    cout << "Empty array handled without crashes" << endl;
    cout << "Test PASSED\n";

    delete data;
}

int main() { 
    cout << "Running tests for lab2\n";
   
    test_min_max_avg_with_positive_numbers();
    test_min_max_avg_with_negative_numbers();
    test_min_max_avg_with_identical_numbers();
    test_min_max_avg_with_single_element();
    test_min_max_avg_with_empty_array();

    cout << "All tests completed successfully!\n";
   
    return 0;
}