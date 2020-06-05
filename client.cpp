#include <winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    WSADATA wsa;

    if (argc != 3)
    {
        printf("Usage: client ip_address port\n");
        return -4;
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

    const char *hostname = argv[1];
    int port = atoi(argv[2]);
    printf("client\n");
    printf("hostname: %s\n", hostname);
    printf("port: %d\n", port);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.S_un.S_addr = inet_addr(hostname);
    if (server.sin_addr.S_un.S_addr == 0xFFFFFFFF)
    {
        struct hostent *host = gethostbyname(hostname);
        if (host == NULL)
        {
            printf("gethostbyname: %d\n", WSAGetLastError());
            return -2;
        }
        server.sin_addr.S_un.S_addr = *(unsigned int *)host->h_addr_list[0];
    }

    error = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (error)
    {
        printf("connect: %d\n", WSAGetLastError());
        return -3;
    }

    char buf[512];
    int len, count;
    for (;;)
    {
        count = recv(sock, buf, sizeof(buf) - 1, 0);
        if (count == 0)
            break;

        buf[count] = 0;
        printf("recv: %s\n", buf);

        if (lstrcmpiA(buf, "quit") == 0)
            break;

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
    }

    closesocket(sock);

    WSACleanup();
    return 0;
}
