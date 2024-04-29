/*
* �̽���_20203082
* �̸� : ESN(Echo Server_Network)
* ����: 
* 
* �ü�� : window11 �� window10
* ���� IDE : visual studio
* �����Ϸ� : visual studio compiler
* 
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ESC 27
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void get_ip(SOCKADDR_IN* addr) {
	char ip[33] = { 0 };



	printf(" ip : %lu\n", INADDR_ANY);
	addr->sin_addr.s_addr = inet_addr(ip);
}

void get_port(SOCKADDR_IN* addr) {
	u_short port = 2001;

	printf(" port : %d\n", port);
	addr->sin_port = htons(port);
}

int server_init() {
	WSADATA wsa_init;
	SOCKET receiver, sender;
	SOCKADDR_IN server_addr, client_addr;

	int client_addr_size, loop = 0;
	char type = 0;
	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init)) {
		printf(" WSA init failed\n");
		return 0;
	}

	//TCP ������� ������� UDP ������� ������� ����
	do {
		loop = 0;

		printf(" Use TCP or UDP\n TCP | T or t\n UDP | U or u\n Back | esc\n\n Enter: ");
		type = _getch();
		fflush(stdin);

		//TCP ���� ����
		if (type == 't' || type == 'T') {

			system("cls");
			printf(" Open TCP socket...  ");

			receiver = socket(AF_INET, SOCK_DGRAM, 0);
		}
		//UDP ���� ����
		else if (type == 'u' || type == 'U') {

			system("cls");
			printf(" Open TCP socket...  ");

			receiver = socket(AF_INET, SOCK_STREAM, 0);
		}
		//�ڷ� ����
		else if (type == ESC) {
			system("cls");
			WSACleanup();
			return 1;
		}
		//�߸��� �Է�
		else {
			system("cls");
			printf(" WRONG INPUT OCCURED\n");
			loop = 1;
		}
	} while (loop);

	if ((int)receiver == -1) printf("failed.\n");
	else {
		printf("success.\n");

		server_addr.sin_family = AF_INET;
		get_ip(&server_addr);
		get_port(&server_addr);

		if (bind(receiver, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
			printf(" Socket bind failed.\n");

			closesocket(receiver);
			WSACleanup();
			return 0;
		}

		while (1) {
			if (listen(receiver, 1) != 0) {
				printf(" Socket listen failed.\n");

				closesocket(receiver);
				WSACleanup();
				return 0;
			}

			sender = accept(receiver, (struct sockaddr*)&client_addr, &client_addr_size);

			if (sender == -1) printf(" Accept failed.\n");
			else {
				//���� �޽��� ó�� ����
			}
		}
		
		closesocket(receiver);
	}
	
	WSACleanup();
	return 0;
}

int client_init() {
	WSADATA wsa_init;
	SOCKET sender;
	int loop = 0;
	char type = 0;
	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init) != 0) {
		printf(" WSA init failed.\n");
		return 0;
	}

	//TCP ������� ������� UDP ������� ������� ����
	do {
		loop = 0;

		printf(" Use TCP or UDP\n TCP | T or t\n UDP | U or u\n Back | esc\n\n Enter: ");
		type = _getch();
		fflush(stdin);

		//TCP ���� ����
		if (type == 't' || type == 'T') {
			
			system("cls");
			printf(" Open TCP socket...  ");

			sender = socket(AF_INET, SOCK_DGRAM, 0);
			
			
		}
		//UDP ���� ����
		else if (type == 'u' || type == 'U') {

			system("cls");
			printf(" Open UDP socket...  ");

			sender = socket(AF_INET, SOCK_STREAM, 0);

			if ((int)sender == -1) {
				printf("failed.\n");
				return 0;
			}

			closesocket(sender);
		}
		//�ڷΰ���
		else if (type == ESC) {
			system("cls");
			WSACleanup();
			return 1;
		}
		//�߸��� �Է�
		else {
			system("cls");
			printf(" WRONG INPUT OCCURED\n");
			loop = 1;
		}
	} while (loop);
	
	if ((int)sender == -1) printf("failed.\n");
	else {
		printf("success.\n");

		while (1) {
			//connect();

		}

		closesocket(sender);
	}
	
	WSACleanup();

	return 0;
}

int main() {
	int loop = 0;
	char sc_select = 0, type = 0;

	//������ �������� Ŭ���̾�Ʈ���� ����
	do {
		loop = 0;

		printf(" Are you a server or a client?\n\n server | S or s\n client | C or c\n End program | esc\n\n ENTER: ");
		sc_select = _getch();
		fflush(stdin);

		if (sc_select == 's' || sc_select == 'S') {
			loop = server_init();
		}
		else if (sc_select == 'c' || sc_select == 'C') {
			loop = client_init();
		}
		else if (sc_select == ESC) {
			break;
		}
		else {
			system("cls");
			printf("WRONG INPUT OCCURED\n");
			loop = 1;
		}
	} while(loop);

	printf("\n Program end");
	return 0;
}
