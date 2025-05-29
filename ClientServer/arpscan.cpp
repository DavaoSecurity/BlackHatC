// ARP scan to find connected devices
// g++ -o arpscan arpscan.cpp
// sudo ./arpscan

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

void scanNetwork(const std::string& subnet, int start, int end, const std::string& outputFile) {
    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error opening output file." << std::endl;
        return;
    }

    outFile << "IP Address\tHostname\n";
    outFile << "-------------------------\n";

    for (int i = start; i <= end; ++i) {
        std::string ip = subnet + std::to_string(i);
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);

        char host[NI_MAXHOST];
        if (getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, sizeof(host), nullptr, 0, 0) == 0) {
            outFile << ip << "\t" << host << "\n";
        } else {
            outFile << ip << "\t" << "Unknown" << "\n";
        }
    }

    outFile.close();
    std::cout << "Scan complete. Results saved to " << outputFile << std::endl;
}

int main() {
    std::string subnet = "192.168.1."; // Change this to your subnet
    int start = 1; // Start of the range
    int end = 254; // End of the range
    std::string outputFile = "network_scan_results.txt";

    scanNetwork(subnet, start, end, outputFile);

    return 0;
}
