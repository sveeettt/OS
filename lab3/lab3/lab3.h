#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

extern std::vector<int> mas;
extern std::vector<bool> isThreadWorks;
extern HANDLE* StopEvent;
extern HANDLE* ResumeEvent;
extern HANDLE* DeleteEvent;
extern HANDLE DeadThEvent;
extern std::mutex cs;
extern std::mutex csout;
extern int n;
extern int thrkol;

void marker(int lpParam);