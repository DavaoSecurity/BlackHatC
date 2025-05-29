// g++ -o cnetscan cnetscan.cpp -lpthread
//sudo ./cnetscan
// to scan all ports 1 to 65535

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <vector>

const int TIMEOUT = 1; // Timeout for socket operations

void scan_tcp(const std::string& ip, int port, std::ofstream& output) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
        output << "TCP Port " << port << " is open\n";
    }
    close(sock);
}

void scan_udp(const std::string& ip, int port, std::ofstream& output) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    // Send a dummy packet
    const char* msg = "ping";
    sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server, sizeof(server));

    char buffer[1024];
    socklen_t len = sizeof(server);
    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    // Try to receive a response
    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, &len) >= 0) {
        output << "UDP Port " << port << " is open\n";
    }
    close(sock);
}

int main() {
    std::string ip;
    std::cout << "Enter the IP address to scan: ";
    std::cin >> ip;

    std::ofstream output("scan_results.txt");
    if (!output.is_open()) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    std::vector<std::thread> threads;

    // Scan TCP ports
    for (int port = 1; port <= 65535; ++port) {
        threads.emplace_back(scan_tcp, ip, port, std::ref(output));
    }

    // Scan UDP ports
    for (int port = 1; port <= 65535; ++port) {
        threads.emplace_back(scan_udp, ip, port, std::ref(output));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    output.close();
    std::cout << "Scanning completed. Results saved to scan_results.txt\n";
    return 0;
}
