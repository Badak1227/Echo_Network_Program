/*
* �̽���_20203082
* �̸� : ESN(Echo_Server_Network)
* ����:
*
* �ü�� : window11 �� window10
* ���� IDE : visual studio
* �����Ϸ� : visual studio compiler
* IPv4
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ESC 27
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int get_ip(SOCKADDR_IN* addr, int type) {
	struct addrinfo* result;
	struct addrinfo hints;
	struct sockaddr_in* local_addr;

	int iresult = 0;
	char myaddr[256];

	if (gethostname(myaddr, sizeof(myaddr)) == -1) return -1;

	printf(" hostname : %s\n", myaddr);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4 �ּҸ� ��� ���� AF_INET ���
	hints.ai_socktype = type;
	hints.ai_protocol = 0;

	iresult = getaddrinfo(myaddr, NULL, &hints, &result);
	if (iresult != 0) {
		printf("%d\n", iresult);
		return -1;
	}
	local_addr = (struct sockaddr_in*)result->ai_addr;

	printf(" ip : %s\n", inet_ntoa(local_addr->sin_addr));
	addr->sin_addr = local_addr->sin_addr;//htonl(INADDR_ANY);

	return 0;
}

void get_port(SOCKADDR_IN* addr) {
	u_short port = 3000;

	printf(" port : %d\n", port);
	addr->sin_port = htons(port);
}

//esc�� Ư�� �Է��� ó���ϱ� ���� ���ڿ� �Է� �Լ�
int get_str(char* msg, int max) {
	int len = 0;
	char ch = 0;

	while (1) {
		ch = _getch();

		//�����̽��ٿ� ���ڵ�
		if (31 < ch && ch < 127) {
			printf("%c", ch);
			msg[len] = ch;

			//���ڿ� �Է� �ִ� ���� 1500;
			if (len < max - 1) len++;
		}
		//�齺���̽�
		else if (len > 0 && ch == 8) {
			printf("\b \b");
			len--;
			msg[len] = 0;
		}
		//���͸� ���� ��� msg ���� ��ȯ
		else if (ch == 13) {
			printf("\n");
			msg[len++] = ch;
			if (len < max) msg[len] = '\0';

			return len;
		}
		//esc�� ���� ��� 0 ��ȯ
		else if (ch == 27) {
			return 0;
		}
	}
}

int tcp_server() {
	WSADATA wsa_init;
	SOCKET server, client;
	SOCKADDR_IN server_addr, client_addr;
	u_long socket_mode = 1; //�񵿱� ���� ���

	int client_addr_size, loop = 0, port = 2000;
	char type = 0, msg[1501] = { 0 };

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init)) {
		printf(" WSA init failed\n");
		return 0;
	}

	system("cls");
	printf(" Open TCP socket...  ");

	server = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(server, FIONBIO, &socket_mode);

	//���� ���� ���� Ȯ��
	if (server == INVALID_SOCKET) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	//bind() �� ����Ȯ��
	server_addr.sin_family = AF_INET;
	//���� ȣ��Ʈ�� IPv4 �ּҸ� ���´�.
	if (get_ip(&server_addr, SOCK_STREAM) == -1) {
		printf(" Get ip address failed\n");

		closesocket(server);
		WSACleanup();
		return 0;
	}

	//��� ������ ��Ʈ��ȣ �ο�
	for (port = 2000; port < 65536; port++) {
		server_addr.sin_port = htons(port);
		if (bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) break;
	}

	if (port == 65536) {
		printf(" Socket bind failed.\n");

		closesocket(server);
		WSACleanup();
		return 0;
	}
	else printf(" port : %d\n", port);

	//listen() �� ���� Ȯ�� - TCP������ listen()�� ���� ���� ��� �ʿ�
	if (listen(server, 5) == -1) {
		printf(" Socket listen failed.\n");

		closesocket(server);
		WSACleanup();
		return 0;
	}

	printf(" Back | esc\n\n");

	client_addr_size = sizeof(client_addr);

	while (1) {
		//esc�� ���� ��� accept ��� ���¿��� ���
		if (_kbhit() && _getch() == 27) break;

		//accept() - TCP������ accept()�� �̿�	
		client = accept(server, (struct sockaddr*)&client_addr, &client_addr_size);

		if (client != -1) printf(" Client %s connected - Disconnect client | esc\n", inet_ntoa(client_addr.sin_addr));

		//Ŭ���̾�Ʈ ������ ������ �� ���� �޽����� ����
		while (client != -1) {
			//esc�� ���� ��� �޽��� ���� ��� ���¿��� ���
			if (_kbhit() && _getch() == 27) {
				printf(" Disconnect client\n");
				break;
			}

			loop = recv(client, msg, 1500, 0);

			//client������ ���� ����
			if (loop == 0) {
				printf(" Client disconnected\n\n");
				break;
			}
			//msg ���Ž� ����
			else if (loop > 0) {
				printf(" received :  %s\n", msg);
				if (send(client, msg, strlen(msg), 0) == -1) {
					printf(" Send message failed\n");
					closesocket(client);
					closesocket(server);
					WSACleanup();
					return 0;
				}
			}
		}

		closesocket(client);
		client = -1;
	}

	closesocket(client);
	closesocket(server);
	WSACleanup();
	system("cls");
	return 1;
}

int udp_server() {
	WSADATA wsa_init;
	SOCKET server, client = -1;
	SOCKADDR_IN server_addr, client_addr;
	u_long socket_mode = 1; //�񵿱� ���� ���

	int client_addr_size, loop = 0, port = 2000;
	char type = 0, msg[1501] = { 0 };


	if (WSAStartup(MAKEWORD(2, 2), &wsa_init)) {
		printf(" WSA init failed\n");
		return 0;
	}

	system("cls");
	printf(" Open UDP socket...  ");

	server = socket(AF_INET, SOCK_DGRAM, 0);
	ioctlsocket(server, FIONBIO, &socket_mode);

	//���� ���� ���� Ȯ��
	if (server == -1) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	//bind() �� ����Ȯ��
	server_addr.sin_family = AF_INET;
	//���� ȣ��Ʈ�� IPv4 �ּҸ� ���´�.
	if (get_ip(&server_addr, SOCK_DGRAM) == -1) {
		printf(" Get ip address failed\n");

		closesocket(server);
		WSACleanup();
		return 0;
	}

	//��밡���� ��Ʈ��ȣ �ο�
	for (port = 2000; port < 65536; port++) {
		server_addr.sin_port = htons(port);
		if (bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) break;
	}

	if (port == 65536) {
		printf(" Socket bind failed.\n");

		closesocket(server);
		WSACleanup();
		return 0;
	}
	else printf(" port : %d\n", port);

	printf(" Back | esc\n\n");


	client_addr_size = sizeof(client_addr);
	while (1) {
		//esc�� ���� ��� �޽��� ���� ��� ���¿��� ���
		if (_kbhit() && _getch() == 27) break;

		//recvfrom�� ���� ���� �޽��� �ٷ� �о���̱�
		if (recvfrom(server, msg, 1500, 0, (struct sockaddr*)&client_addr, &client_addr_size) != -1) {
			printf(" received :  %s  FROM %s\n", msg, inet_ntoa(client_addr.sin_addr));

			if (sendto(server, msg, strlen(msg), 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
				printf(" Send message failed\n");
				closesocket(client);
				closesocket(server);
				WSACleanup();
				return 0;
			}

			closesocket(client);
			client = -1;
		}
	}

	closesocket(client);
	closesocket(server);
	WSACleanup();
	system("cls");
	return 1;
}

int tcp_client() {
	WSADATA wsa_init;
	SOCKET client;
	SOCKADDR_IN server_addr;
	u_long socket_mode = 1; //�񵿱� ���� ���

	int server_addr_size, loop = 0, len = 0, server_port = 1025;
	char type = 0, msg[1501] = { 0 }, server_ip[40] = { 0 };
	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init) != 0) {
		printf(" WSA init failed.\n");
		return 0;
	}

	printf(" Open TCP socket...  ");

	client = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(client, FIONBIO, &socket_mode);

	//���� ���� ���� Ȯ��
	if (client == -1) {
		printf("failed.\n");

		closesocket(client);
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	//���� ip�� port ��ȣ �Է�
	printf(" Enter server ip : ");
	scanf("%s", server_ip);
	printf(" Enter server port : ");
	scanf("%d", &server_port);
	while (getchar() != '\n');

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons((u_short)server_port);

	//������ connect ����
	if (connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1);

	while (1) {
		printf(" send :  ");
		len = get_str(msg, 1500);

		printf("%d %s\n", len, msg);

		if (len == 0) break;

		if (send(client, msg, len + 1, 0) == -1) {
			printf(" Send message failed\n");
			closesocket(client);
			WSACleanup();
			return 0;
		}

		while (1) {
			if (recv(client, msg, 1500, 0) != -1) {
				printf(" received :  %s\n\n", msg);
				break;
			}
		}
	}

	closesocket(client);
	WSACleanup();
	system("cls");
	return 1;
}

int udp_client() {
	WSADATA wsa_init;
	SOCKET client;
	SOCKADDR_IN server_addr;
	u_long socket_mode = 1; //�񵿱� ���� ���

	int server_addr_size, len = 0, loop = 0, server_port = 1025;
	char type = 0, msg[1501] = { 0 }, server_ip[40] = { 0 };
	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init) != 0) {
		printf(" WSA init failed.\n");
		return 0;
	}

	printf(" Open UDP socket...  ");

	client = socket(AF_INET, SOCK_DGRAM, 0);
	ioctlsocket(client, FIONBIO, &socket_mode);

	//���� ���� ���� Ȯ��
	if (client == -1) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	//���� ip�� port ��ȣ �Է�
	printf("Enter server ip : ");
	scanf("%s", server_ip);
	printf("Enter server port : ");
	scanf("%d", &server_port);
	while (getchar() != '\n');

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons((u_short)server_port);

	server_addr_size = sizeof(server_addr);


	while (1) {
		printf(" send :  ");

		len = get_str(msg, 1500);

		if (len == 0) break;

		if (sendto(client, msg, len + 1, 0, (struct sockaddr*)&server_addr, server_addr_size) == -1) {
			printf(" Send message failed\n");
			closesocket(client);
			WSACleanup();
			return 0;
		}

		while (1) {
			if (recvfrom(client, msg, 1500, 0, (struct sockaddr*)&server_addr, &server_addr_size) != -1) {
				printf(" received :  %s\n", msg);
			}
		}
	}
}

int main() {
	int loop = 1;
	char sc_select = 0, type = 0;

	system("cls");

	//������ �������� Ŭ���̾�Ʈ���� ����
	do {
		printf(" Are you a server or a client?\n\n server | S or s\n client | C or c\n End program | esc\n\n ENTER: ");
		sc_select = _getch();

		if (sc_select == 's' || sc_select == 'S') {
			system("cls");

			//TCP ������� ������� UDP ������� ������� ����
			do {
				printf(" Use TCP or UDP\n\n TCP | T or t\n UDP | U or u\n Back | esc\n\n Enter: ");
				type = _getch();

				//TCP ���� ����
				if (type == 't' || type == 'T') loop = tcp_server();
				//UDP ���� ����
				else if (type == 'u' || type == 'U') loop = udp_server();
				//�ڷ� ����
				else if (type == ESC) {
					system("cls");
					break;
				}
				//�߸��� �Է�
				else {
					system("cls");
					printf(" WRONG INPUT OCCURED\n");
				}
			} while (loop);
		}
		else if (sc_select == 'c' || sc_select == 'C') {
			system("cls");

			//TCP ������� ������� UDP ������� ������� ����
			do {
				printf(" Use TCP or UDP\n\n TCP | T or t\n UDP | U or u\n Back | esc\n\n Enter: ");
				type = _getch();

				//TCP ���� ����
				if (type == 't' || type == 'T') loop = tcp_client();
				//UDP ���� ����
				else if (type == 'u' || type == 'U') loop = udp_client();
				//�ڷΰ���
				else if (type == ESC) {
					system("cls");
					break;
				}
				//�߸��� �Է�
				else {
					system("cls");
					printf(" WRONG INPUT OCCURED\n");
				}
			} while (loop);
		}
		else if (sc_select == ESC) {
			break;
		}
		else {
			system("cls");
			printf(" WRONG INPUT OCCURED\n");
		}
	} while (loop);

	printf("\n Program end");
	return 0;
}