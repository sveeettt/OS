#include "lab3.h"

using namespace std;

vector<int> mas;
vector<bool> isThreadWorks;
HANDLE* StopEvent;
HANDLE* ResumeEvent;
HANDLE* DeleteEvent;
HANDLE DeadThEvent;
mutex cs;
mutex csout;
int n = 0;
int thrkol = 0;

void marker(int lpParam) {
    srand(lpParam + 1); // +1 чтобы избежать нулевого seed
    int elmarked = 0;
    int threadIndex = lpParam;

    while (true) {
        int index = rand() % n;

        cs.lock();
        if (mas[index] == 0) {  // »зменено с -1 на 0, т.к. массив инициализируетс€ нул€ми
            Sleep(5);
            mas[index] = lpParam + 1;  // +1 чтобы отличать от нул€
            elmarked++;
            cs.unlock();
            Sleep(5);
        }
        else {
            cs.unlock();

            csout.lock();
            cout << "Thread number " << threadIndex + 1
                << "\nNumber of marked elements: " << elmarked
                << " elements.\nCan't mark index " << index << endl;
            SetEvent(StopEvent[lpParam]);
            csout.unlock();

            while (true) {
                if (WaitForSingleObject(DeleteEvent[lpParam], 0) != WAIT_TIMEOUT) {
                    cs.lock();
                    isThreadWorks[lpParam] = false;
                    for (int i = 0; i < n; ++i) {
                        if (mas[i] == lpParam + 1) {
                            mas[i] = 0;
                        }
                    }
                    thrkol--;
                    cs.unlock();
                    SetEvent(DeadThEvent);
                    return;
                }
                else if (WaitForSingleObject(ResumeEvent[lpParam], 0) != WAIT_TIMEOUT) {
                    ResetEvent(ResumeEvent[lpParam]);
                    break;
                }
            }
        }
    }
}

int main() {
    cout << "Enter size:\n";
    cin >> n;
    mas = vector<int>(n, 0);

    int thkol;
    cout << "Enter threads amount:\n";
    cin >> thkol;
    thrkol = thkol;

    thread* hThreads = new thread[thkol];
    StopEvent = new HANDLE[thkol];
    ResumeEvent = new HANDLE[thkol];
    DeleteEvent = new HANDLE[thkol];
    isThreadWorks.resize(thkol, true);

    DeadThEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (int i = 0; i < thkol; i++) {
        StopEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        ResumeEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        DeleteEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hThreads[i] = thread(marker, i);
    }

    while (thrkol > 0) {
        // ∆дем, пока все потоки не подадут сигналы
        WaitForMultipleObjects(thkol, StopEvent, TRUE, INFINITE);

        cout << "\nArray before deleting: ";
        for (int i = 0; i < n; i++) {
            cout << mas[i] << " ";
        }
        cout << endl;

        int thrId;
        cout << "Enter id of thread that need to stop (1-" << thkol << "): ";
        cin >> thrId;
        thrId--; // переводим в 0-индексацию

        while (thrId < 0 || thrId >= thkol || !isThreadWorks[thrId]) {
            cout << "This thread does not exist or already stopped, enter another: ";
            cin >> thrId;
            thrId--;
        }

        ResetEvent(DeadThEvent);
        SetEvent(DeleteEvent[thrId]);
        WaitForSingleObject(DeadThEvent, INFINITE);

        cout << "Array after deletion: ";
        for (int i = 0; i < n; i++) {
            cout << mas[i] << " ";
        }
        cout << endl;

        // ƒаем сигнал на продолжение оставшимс€ потокам
        for (int i = 0; i < thkol; i++) {
            if (isThreadWorks[i]) {
                ResetEvent(StopEvent[i]);
                SetEvent(ResumeEvent[i]);
            }
        }
    }

    // ќжидаем завершени€ всех потоков
    for (int i = 0; i < thkol; i++) {
        if (hThreads[i].joinable()) {
            hThreads[i].join();
        }
    }

    // ќчистка ресурсов
    for (int i = 0; i < thkol; i++) {
        CloseHandle(StopEvent[i]);
        CloseHandle(ResumeEvent[i]);
        CloseHandle(DeleteEvent[i]);
    }
    CloseHandle(DeadThEvent);
    delete[] hThreads;
    delete[] StopEvent;
    delete[] ResumeEvent;
    delete[] DeleteEvent;

    return 0;
}