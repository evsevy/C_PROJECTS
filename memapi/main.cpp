#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

struct ProcessInfo {
    std::wstring name;
    DWORD memoryUsed;
};

bool CompareByMemoryUsed(const ProcessInfo& a, const ProcessInfo& b) {
    return a.memoryUsed < b.memoryUsed;
}

int main() {
    std::vector<ProcessInfo> processes;
    DWORD aProcesses[1024], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
            if (hProcess != NULL) {
                TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));

                    DWORD cbMemoryUsed;
                    PROCESS_MEMORY_COUNTERS pmc;
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        cbMemoryUsed = pmc.WorkingSetSize;
                        ProcessInfo process = { szProcessName, cbMemoryUsed };
                        processes.push_back(process);
                    }
                }

                CloseHandle(hProcess);
            }
        }
    }

    std::sort(processes.begin(), processes.end(), CompareByMemoryUsed);

    for (const ProcessInfo& process : processes) {
        std::wcout << "Process Name: " << process.name << std::endl;
        std::wcout << "Memory Used: " << process.memoryUsed << " bytes" << std::endl << std::endl;
    }

    std::wcout << "Press any key to restart the program..." << std::endl;
    _getwch();
    system("cls");

    return main();
}
