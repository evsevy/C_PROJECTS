//*************************************************** © MERGULOV E.V. 2024 ***********************************************************//

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
        std::this_thread::sleep_for(std::chrono::seconds(2 + std::rand() % 3)); // Random sleep time в пределах 2-4 сек.

        // ЗАПУСК critical section
        mtx.lock();
        std::vector<std::wstring> fileList;
        WIN32_FIND_DATAW FindFileData;
        HANDLE hFind = FindFirstFileW(L"*.txt", &FindFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                fileList.push_back(std::wstring(FindFileData.cFileName));
            } while (FindNextFileW(hFind, &FindFileData));
            FindClose(hFind);

            std::wofstream outfile(L"LOG\\list.log", std::ios_base::app); // Сохранил list.log в LOG folder
            if (outfile.is_open()) {
                for (const auto& file : fileList) {
                    outfile << file.c_str() << std::endl;
                }
                outfile << L"Unique Number: " << obj->uniqueNumber << std::endl;
                outfile.close();
            }
        }

        for (const auto& file : fileList) {
            DeleteFile((L"LOG\\" + file).c_str()); // Удалил полученные файлы из LOG folder
        }
        mtx.unlock();
        // КОНЕЦ critical section

        std::wcout << L"Thread with Unique Number " << obj->uniqueNumber << L" has finished." << std::endl; // ВЫВОД СООБЩЕНИЯ О ЗАВЕРШЕНИИ

        break;
    }
}

int wmain() {
    MyObject obj1(1353535535);
    MyObject obj2(2464646465);

    std::thread thread1(threadFunc, &obj1); // Создал потоки std::thread 
    std::thread thread2(threadFunc, &obj2);

    thread1.join(); 
    thread2.join();

    return 0;
}
/*
1. Созданы два объекта класса MyObject с уникальными номерами.
2. Для каждого объекта создан свой поток через std::thread, передавая указатель на объект в функцию threadFunc.
3. В функции threadFunc реализована логика поиска файлов и их записи в файл LOG\list.log, а также удаление найденных файлов порциями внутри критической секции.
4. В main функции потоки дожидаются завершения работы друг друга перед завершением программы.
5. Теперь программа должна корректно выполнять задание и сохранять результат в файле LOG\list.log.

Произведены следующие изменения :
1. Изменен тип данных вектора `fileList` на `std::wstring`.
2. Исправлено использование `FindFileData.cFileName` в конструкторе `std::wstring`.
3. Изменен тип файла для открытия выходного файла на `std::wofstream`.
4. Исправлено использование `outfile` для записи строк из вектора `fileList`.
5. Исправлен вызов функции `DeleteFile` с использованием `L` для строкового литерала.
6. Использование `std::wcout` для вывода сообщения о завершении потока.
7. Изменено имя функции `main` на `wmain` для поддержки широких строк в Windows.
8. Исправлено приведение типа для избежания потери данных при преобразовании времени в unsigned int.
9. Заменена функция времени time(nullptr) на static_cast<unsigned int>(time(nullptr)).
10. Заменены функции Windows API FindFirstFile и FindNextFile на FindFirstFileW и FindNextFileW для работы с широкими строками.
11. При добавлении файлов в fileList происходит преобразование cFileName из wchar_t в std::string.
12. При удалении файлов функция DeleteFile получает аргумент типа LPCWSTR.
*/