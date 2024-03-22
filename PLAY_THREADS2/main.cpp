//*************************************************** © MERGULOV E.V. 2024 ***********************************************************//


#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <mutex>
#include <codecvt>
#include <random>
#include <algorithm>
#include <iterator>
using namespace std;

// Класс с числовым полем
class MyClass {
private:
    int uniqueNumber;
public:
    MyClass(int number) : uniqueNumber(number) {}

    int getUniqueNumber() const {
        return uniqueNumber;
    }
};

// Глобальная критическая секция для обеспечения безопасности доступа к ресурсам
std::mutex mtx;

// Функция для поиска и обработки файлов внутри потока
DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    MyClass* myObject = (MyClass*)lpParam;

    while (true) {
        // Генерируем случайную задержку от 2 до 4 секунд
        int delay = rand() % 3 + 2;
        Sleep(delay * 1000);

        mtx.lock();

        WIN32_FIND_DATAW FindFileData;
        HANDLE hFind;

        hFind = FindFirstFileW(L".\\*.txt", &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            std::vector<std::wstring> fileList;

            do {
                fileList.push_back(FindFileData.cFileName);
            } while (FindNextFileW(hFind, &FindFileData) != 0);

            FindClose(hFind);

            // Записываем список файлов в файл list.log
            std::wofstream outFile("list.log", std::ios::app);
            if (outFile.is_open()) {
                for (const auto& fileName : fileList) {
                    outFile << fileName.c_str() << std::endl; // Исправлено
                }
                outFile << L"Thread unique number: " << myObject->getUniqueNumber() << std::endl;
                outFile.close();
            }

            // Удаляем найденные файлы
            for (const auto& fileName : fileList) {
                std::wstring fullPath = L".\\" + fileName;
                if (DeleteFileW(fullPath.c_str()) == 0) {
                    std::cerr << "Error deleting file: " << fileName.c_str() << std::endl; // Исправлено
                }
            }
        }

        mtx.unlock();

        break;
    }

    return 0;
}

int main() {
    // Создаем два объекта класса MyClass
    MyClass object1(19875475654547);
    MyClass object2(29879678678756);

    // Создаем два потока
    HANDLE hThread1, hThread2;

    hThread1 = CreateThread(NULL, 0, ThreadFunc, &object1, 0, NULL);
    if (hThread1 == NULL) {
        std::cerr << "Error when creating thread 1" << std::endl;
        return 1;
    }

    hThread2 = CreateThread(NULL, 0, ThreadFunc, &object2, 0, NULL);
    if (hThread2 == NULL) {
        TerminateThread(hThread1, 0); // Принудительно завершаем первый поток
        CloseHandle(hThread1);
        std::cerr << "Error when creating thread 2" << std::endl;
        return 1;
    }

    // Ожидаем завершения работы потоков
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    // Закрываем дескрипторы потоков
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    return 0;
}
