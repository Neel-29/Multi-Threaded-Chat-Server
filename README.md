# Multi-Threaded-Chat-Server

Version requirements: g++ -11 

Class Diagram:

  ChatServer
    - std::vector<ClientHandler> clients
    + startServer()
    + stopServer()
    + broadcastMessage(string message, int senderId)
    + handleClient(int clientSocket)

  ClientHandler
    - int clientSocket
    - ChatServer& server
    + run()
    + sendMessage(string message)
    + receiveMessage()
  
  Client
    - string username
    - int socket
    + connectToServer()
    + sendMessage(string message)
    + receiveMessage()

Testing the Project
Compile the Server and Client:

bash
g++ -o chat_server Multi-Threaded Chat Server_Server mode.cpp -pthread
g++ -o chat_client Multi-Threaded Chat Server_Client mode.cpp
Run the Server:

bash
./chat_server
Run Multiple Clients:

bash
./chat_client

Enhancements
User Authentication: Add registration and login functionality.
Private Messaging: Allow users to send private messages.
GUI: Develop a graphical user interface for the client using a library like Qt.
Encryption: Secure the communication using encryption techniques.

On Windows with Visual Studio:
Ensure your project is set to use C++11 or later in the project properties.

By following these steps, you should be able to resolve the issues related to std::thread and std::mutex and successfully compile and run your multi-threaded chat server and client.
