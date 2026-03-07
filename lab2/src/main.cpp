#include <iostream>
#include <vector>
#include <Windows.h>
#include "Threads.h"
#include "Data.h"
using namespace std;

int main() {
    vector<int> arr;
    int n;

    cout << "Enter array size\n";
    cin >> n;

    arr.resize(n);

    cout << "Enter array elements\n";
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    Data* data = new Data(arr);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, (void*)data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, (void*)data, 0, NULL);

    if (hMinMax == NULL || hAverage == NULL) {
        cout << "Failed to create threads. Error: " << GetLastError() << endl;
        delete data;
        return 1;
    }

    cout << "Waiting for min_max thread..." << endl;
    WaitForSingleObject(hMinMax, INFINITE);

    cout << "Waiting for average thread..." << endl;
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    cout << "Replacing min and max with average: " << data->average << endl;
    arr[data->min] = static_cast<int>(data->average);
    arr[data->max] = static_cast<int>(data->average);

    cout << "Changed array: ";
    for (size_t i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    delete data;

    system("pause");
    return 0;
}