//*************************************************** � MERGULOV E.V. 2024 ***********************************************************//


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

// ����� � �������� �����
class MyClass {
private:
    int uniqueNumber;
public:
    MyClass(int number) : uniqueNumber(number) {}

    int getUniqueNumber() const {
        return uniqueNumber;
    }
};

// ���������� ����������� ������ ��� ����������� ������������ ������� � ��������
std::mutex mtx;

// ������� ��� ������ � ��������� ������ ������ ������
DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    MyClass* myObject = (MyClass*)lpParam;

    while (true) {
        // ���������� ��������� �������� �� 2 �� 4 ������
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

            // ���������� ������ ������ � ���� list.log
            std::wofstream outFile("list.log", std::ios::app);
            if (outFile.is_open()) {
                for (const auto& fileName : fileList) {
                    outFile << fileName.c_str() << std::endl; // ����������
                }
                outFile << L"Thread unique number: " << myObject->getUniqueNumber() << std::endl;
                outFile.close();
            }

            // ������� ��������� �����
            for (const auto& fileName : fileList) {
                std::wstring fullPath = L".\\" + fileName;
                if (DeleteFileW(fullPath.c_str()) == 0) {
                    std::cerr << "Error deleting file: " << fileName.c_str() << std::endl; // ����������
                }
            }
        }

        mtx.unlock();

        break;
    }

    return 0;
}

int main() {
    // ������� ��� ������� ������ MyClass
    MyClass object1(19875475654547);
    MyClass object2(29879678678756);

    // ������� ��� ������
    HANDLE hThread1, hThread2;

    hThread1 = CreateThread(NULL, 0, ThreadFunc, &object1, 0, NULL);
    if (hThread1 == NULL) {
        std::cerr << "Error when creating thread 1" << std::endl;
        return 1;
    }

    hThread2 = CreateThread(NULL, 0, ThreadFunc, &object2, 0, NULL);
    if (hThread2 == NULL) {
        TerminateThread(hThread1, 0); // ������������� ��������� ������ �����
        CloseHandle(hThread1);
        std::cerr << "Error when creating thread 2" << std::endl;
        return 1;
    }

    // ������� ���������� ������ �������
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    // ��������� ����������� �������
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    return 0;
}
