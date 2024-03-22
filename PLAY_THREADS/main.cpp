//*************************************************** � MERGULOV E.V. 2024 ***********************************************************//

#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <vector>
#include <thread>
#include <mutex>

std::mutex mtx;

class MyObject {
public:
    MyObject(int num) : uniqueNumber(num), fileName("file" + std::to_string(num) + ".txt") {}

    int uniqueNumber;
    std::string fileName;
};

void threadFunc(MyObject* obj) {
    std::srand(static_cast<unsigned int>(time(nullptr)));
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2 + std::rand() % 3)); // Random sleep time � �������� 2-4 ���.

        // ������ critical section
        mtx.lock();
        std::vector<std::wstring> fileList;
        WIN32_FIND_DATAW FindFileData;
        HANDLE hFind = FindFirstFileW(L"*.txt", &FindFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                fileList.push_back(std::wstring(FindFileData.cFileName));
            } while (FindNextFileW(hFind, &FindFileData));
            FindClose(hFind);

            std::wofstream outfile(L"LOG\\list.log", std::ios_base::app); // �������� list.log � LOG folder
            if (outfile.is_open()) {
                for (const auto& file : fileList) {
                    outfile << file.c_str() << std::endl;
                }
                outfile << L"Unique Number: " << obj->uniqueNumber << std::endl;
                outfile.close();
            }
        }

        for (const auto& file : fileList) {
            DeleteFile((L"LOG\\" + file).c_str()); // ������ ���������� ����� �� LOG folder
        }
        mtx.unlock();
        // ����� critical section

        std::wcout << L"Thread with Unique Number " << obj->uniqueNumber << L" has finished." << std::endl; // ����� ��������� � ����������

        break;
    }
}

int wmain() {
    MyObject obj1(1353535535);
    MyObject obj2(2464646465);

    std::thread thread1(threadFunc, &obj1); // ������ ������ std::thread 
    std::thread thread2(threadFunc, &obj2);

    thread1.join(); 
    thread2.join();

    return 0;
}
/*
1. ������� ��� ������� ������ MyObject � ����������� ��������.
2. ��� ������� ������� ������ ���� ����� ����� std::thread, ��������� ��������� �� ������ � ������� threadFunc.
3. � ������� threadFunc ����������� ������ ������ ������ � �� ������ � ���� LOG\list.log, � ����� �������� ��������� ������ �������� ������ ����������� ������.
4. � main ������� ������ ���������� ���������� ������ ���� ����� ����� ����������� ���������.
5. ������ ��������� ������ ��������� ��������� ������� � ��������� ��������� � ����� LOG\list.log.

����������� ��������� ��������� :
1. ������� ��� ������ ������� `fileList` �� `std::wstring`.
2. ���������� ������������� `FindFileData.cFileName` � ������������ `std::wstring`.
3. ������� ��� ����� ��� �������� ��������� ����� �� `std::wofstream`.
4. ���������� ������������� `outfile` ��� ������ ����� �� ������� `fileList`.
5. ��������� ����� ������� `DeleteFile` � �������������� `L` ��� ���������� ��������.
6. ������������� `std::wcout` ��� ������ ��������� � ���������� ������.
7. �������� ��� ������� `main` �� `wmain` ��� ��������� ������� ����� � Windows.
8. ���������� ���������� ���� ��� ��������� ������ ������ ��� �������������� ������� � unsigned int.
9. �������� ������� ������� time(nullptr) �� static_cast<unsigned int>(time(nullptr)).
10. �������� ������� Windows API FindFirstFile � FindNextFile �� FindFirstFileW � FindNextFileW ��� ������ � �������� ��������.
11. ��� ���������� ������ � fileList ���������� �������������� cFileName �� wchar_t � std::string.
12. ��� �������� ������ ������� DeleteFile �������� �������� ���� LPCWSTR.
*/