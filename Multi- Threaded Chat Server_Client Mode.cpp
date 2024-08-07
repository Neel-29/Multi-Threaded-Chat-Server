#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif
#include <iostream>
#include <thread>

class Client {
public:
    void connectToServer(const std::string& serverIp, int port);
    void sendMessage(const std::string& message);
    void receiveMessages();

private:
    int clientSocket;
};

void Client::connectToServer(const std::string& serverIp, int port) {
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return;
    }

    std::thread recvThread(&Client::receiveMessages, this);
    recvThread.detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        sendMessage(message);
    }
}

void Client::sendMessage(const std::string& message) {
    send(clientSocket, message.c_str(), message.size(), 0);
}

void Client::receiveMessages() {
    char buffer[1024] = {0};
    while (true) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) break;
        std::cout << std::string(buffer, bytesRead) << std::endl;
    }

    #ifdef _WIN32
        closesocket(clientSocket);
        WSACleanup();
    #else
        close(clientSocket);
    #endif
}

int main() {
    Client client;
    client.connectToServer("127.0.0.1", 8080);
    return 0;
}
