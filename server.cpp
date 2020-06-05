#include <winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    WSADATA wsa;

    if (argc != 2)
    {
        printf("Usage: server port\n");
        return -2;
    }

    int error = WSAStartup(MAKEWORD(2, 0), &wsa);
    switch (error)
    {
    case 0:
        break;
    case WSASYSNOTREADY:
        printf("WSASYSNOTREADY\n");
        return -1;
    case WSAVERNOTSUPPORTED:
        printf("WSAVERNOTSUPPORTED\n");
        return -1;
    case WSAEINPROGRESS:
        printf("WSAEINPROGRESS\n");
        return -1;
    case WSAEPROCLIM:
        printf("WSAEPROCLIM\n");
        return -1;
    case WSAEFAULT:
        printf("WSAEFAULT\n");
        return -1;
    }

    int port = atoi(argv[1]);
    printf("server\n");
    printf("port: %d\n", port);

    SOCKET sock0 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock0 == INVALID_SOCKET)
    {
        printf("socket: %d\n", WSAGetLastError());
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        printf("bind: %d\n", WSAGetLastError());
        return 2;
    }

    if (listen(sock0, 5) != 0)
    {
        printf("bind: %d\n", WSAGetLastError());
        return 3;
    }

    struct sockaddr_in client;
    int len = sizeof(client);
    SOCKET sock = accept(sock0, (struct sockaddr *)&client, &len);
    if (sock == INVALID_SOCKET)
    {
        printf("accept: %d\n", WSAGetLastError());
        return 4;
    }

    char buf[512];
    int count;
    for (;;)
    {
        printf("send> ");
        fgets(buf, sizeof(buf), stdin);
        StrTrimA(buf, "\r\n");
        len = lstrlenA(buf);

        count = send(sock, buf, len, 0);
        if (count <= 0)
        {
            printf("send: %d\n", WSAGetLastError());
            break;
        }

        if (lstrcmpiA(buf, "quit") == 0)
            break;

        count = recv(sock, buf, sizeof(buf) - 1, 0);
        if (count == 0)
            break;

        buf[count] = 0;
        printf("recv: %s\n", buf);

        if (lstrcmpiA(buf, "quit") == 0)
            break;
    }

    shutdown(sock, SD_BOTH);
    closesocket(sock);

    WSACleanup();
    return 0;
}
