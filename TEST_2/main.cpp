#include <iostream>
#include <windows.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <string>

// Класс с одним числовым полем
class MyClass {
public:
    uint64_t uniqueNumber;

    MyClass(uint64_t number) : uniqueNumber(number) {}
};

// Глобальная критическая секция
CRITICAL_SECTION g_cs;

// Функция для потока
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    MyClass* myClass = (MyClass*)lpParam;

    while (true) {
        Sleep((rand() % 3 + 2) * 1000); // Рандомная пауза 2-4 секунды

        EnterCriticalSection(&g_cs);

        WIN32_FIND_DATAW FindFileData;
        HANDLE hFind = FindFirstFileW(L"*.txt", &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            std::vector<std::wstring> fileList;
            do {
                fileList.push_back(FindFileData.cFileName);
            } while (FindNextFileW(hFind, &FindFileData) != 0);

            // Запись в файл list.log
            std::wofstream file(L"list.log", std::ios_base::app);
            if (file.is_open()) {
                for (const auto& fileName : fileList) {
                    file << fileName << std::endl;
                }
                file << myClass->uniqueNumber << std::endl;

                file.close();
            }

            FindClose(hFind);

            for (const auto& fileName : fileList) {
                std::wstring filePath = fileName;
                DeleteFileW(filePath.c_str()); // Удаление найденных файлов
            }

            LeaveCriticalSection(&g_cs);
            break;
        }

        LeaveCriticalSection(&g_cs);
    }

    return 0;
}

int main() {
    InitializeCriticalSection(&g_cs); // Инициализация критической секции

    srand(time(NULL));

    MyClass obj1(111); // Создание объекта 1 с комбинацией чисел
    MyClass obj2(222); // Создание объекта 2 с комбинацией чисел

    HANDLE hThread1 = CreateThread(NULL, 0, ThreadFunction, &obj1, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, ThreadFunction, &obj2, 0, NULL);

    HANDLE threads[2] = { hThread1, hThread2 };

    WaitForMultipleObjects(2, threads, TRUE, INFINITE); // Ожидание завершения работы потоков

    for (int i = 0; i < 2; i++) {
        if (threads[i] != NULL) {
            CloseHandle(threads[i]);
        }
    }

    DeleteCriticalSection(&g_cs); // Удаление критической секции

    return 0;
}