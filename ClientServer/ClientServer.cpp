#define WIN32_LEAN_AND_MEAN //macros
//libraries
#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; //структура для хранения информации windows socket
    ADDRINFO hints; //для хранения инфы сокета
    ADDRINFO* addrResult; //
    SOCKET ConnectSocket = INVALID_SOCKET; //создаем сокет для соединения но пока не даем ему инфы
    char recvBuffer[512]; //переменная для полученной инфы

    const char* sendBuffer1 = "Hello from client 1"; //переменная для отправления инфы
    const char* sendBuffer2 = "Hello from client 2";

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //старт использования библиотеки сокетов
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    } //проверка что библиотека была начата корректно

    ZeroMemory(&hints, sizeof(hints)); //создание памяти с нулевыми параметрами
    hints.ai_family = AF_INET; //4бит инет
    hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP; // Используем протокол TCP

    result = getaddrinfo("localhost", "666", &hints, &addrResult); //запихивание данных о сокете в переменную сокета 
    //адрес, порт,семейство структуры, адрес сокета
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //проверка на правильное инициализацию структуры

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    //создание сокета для соединения
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //проверка на правильное создание сокета
        // Если создание сокета завершилось с ошибкой также освобождаем память, выделенную под структуру addr

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    // Привязываем сокет к IP-адресу (при успешном соединении с сервером)
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    // Если привязать адрес к сокету не удалось, то выводим сообщение
    // об ошибке, освобождаем память, выделенную под структуру addr.
    // и закрываем открытый сокет.
    // Выгружаем DLL-библиотеку из памяти и закрываем программу.

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    //отправляем первое сообщение (инфа сокета, само соо, длина соо, количество флагов
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    //если отправить не получилось, закрываем сокет, освобождаем память выделенную под addr
    // выгружаем бибилиотеку и закрываем программу

    cout << "Sent: " << result << " bytes" << endl;

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = shutdown(ConnectSocket, SD_SEND); //закрываем возможность отправлять и получать новые данные, но оставляем возможность 
    //получать уже отправленные в нашу сторону сообщения
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //при ошибке шатдауна полностью закрываем сокет, освобождаем инфу под аддр, выгружаем библиотеку, закрываем программу

    do {
        ZeroMemory(recvBuffer, 512); //в память загружаем полученную инфу
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        } //выводим в консоль полученную инфу
        else if (result == 0) {
            cout << "Connection closed" << endl;
        } //если инфы не было получено, выводим эту строку 
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        } //если ошибка, выводим эту строку и ошибку
    } while (result > 0);

    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
    //закрываем сокет, освобождаем память для аддр, выгружаем библиотеку, закрываем программу
}