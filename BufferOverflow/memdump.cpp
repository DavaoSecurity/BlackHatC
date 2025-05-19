// memory dump by Nathan W Jones nat@davaosecurity.com
// This code requires administrative privileges to run.

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>

void DumpProcessMemory(DWORD processID, const std::string& outputFile) {
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Could not open process: " << GetLastError() << std::endl;
        return;
    }

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Could not open output file." << std::endl;
        CloseHandle(hProcess);
        return;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    MEMORY_BASIC_INFORMATION memInfo;
    unsigned char* address = 0;

    while (address < sysInfo.lpMaximumApplicationAddress) {
        if (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo))) {
            if (memInfo.State == MEM_COMMIT && memInfo.Protect != PAGE_NOACCESS) {
                std::vector<char> buffer(memInfo.RegionSize);
                SIZE_T bytesRead;
                if (ReadProcessMemory(hProcess, address, buffer.data(), memInfo.RegionSize, &bytesRead)) {
                    outFile.write(buffer.data(), bytesRead);
                }
            }
            address += memInfo.RegionSize;
        } else {
            break;
        }
    }

    outFile.close();
    CloseHandle(hProcess);
    std::cout << "Memory dump completed." << std::endl;
}

DWORD GetProcessID(const std::string& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_PROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (processName == pe.szExeFile) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    std::string processName;
    std::string outputFile;

    std::cout << "Enter the process name (e.g., notepad.exe): ";
    std::cin >> processName;
    std::cout << "Enter the output file name (e.g., dump.bin): ";
    std::cin >> outputFile;

    DWORD processID = GetProcessID(processName);
    if (processID == 0) {
        std::cerr << "Process not found." << std::endl;
        return 1;
    }

    DumpProcessMemory(processID, outputFile);
    return 0;
}
