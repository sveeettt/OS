#ifndef THREADS_H
#define THREADS_H

#include <iostream>
#include <vector>
#include <Windows.h>
#include "Data.h"

DWORD WINAPI min_max(LPVOID ArrData);
DWORD WINAPI average(LPVOID ArrData);

#endif