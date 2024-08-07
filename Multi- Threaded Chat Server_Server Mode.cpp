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
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

class ClientHandler;

class ChatServer {
public:
    void startServer(int port);
    void stopServer();
    void broadcastMessage(const std::string& message, int senderId);

private:
    int serverSocket;
    std::vector<std::thread> clientThreads;
    std::vector<ClientHandler*> clients;
    std::mutex clientsMutex;

    void handleClient(int clientSocket);
};

class ClientHandler {
public:
    ClientHandler(int clientSocket, ChatServer& server)
        : clientSocket(clientSocket), server(server) {}

    void run();
    void sendMessage(const std::string& message);
    std::string receiveMessage();
    int getClientId() { return clientSocket; } // Add this getter

private:
    int clientSocket;
    ChatServer& server;
};

void ChatServer::startServer(int port) {
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif
    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return;
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(new ClientHandler(clientSocket, *this));
        clientThreads.emplace_back(&ClientHandler::run, clients.back());
    }
}

void ChatServer::stopServer() {
    #ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
    #else
        close(serverSocket);
    #endif

    for (auto& thread : clientThreads) {
        thread.join();
    }
}

void ChatServer::broadcastMessage(const std::string& message, int senderId) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        if (client->getClientId() != senderId) {
            client->sendMessage(message);
        }
    }
}

void ClientHandler::run() {
    while (true) {
        std::string message = receiveMessage();
        if (message.empty()) break;
        server.broadcastMessage(message, clientSocket);
    }

    #ifdef _WIN32
        closesocket(clientSocket);
    #else
        close(clientSocket);
    #endif
}

void ClientHandler::sendMessage(const std::string& message) {
    send(clientSocket, message.c_str(), message.size(), 0);
}

std::string ClientHandler::receiveMessage() {
    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) return "";
    return std::string(buffer, bytesRead);
}

int main() {
    ChatServer server;
    server.startServer(8080);
    return 0;
}
