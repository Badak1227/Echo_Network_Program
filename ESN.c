/*
* 이승진_20203082
* 이름 : ESN(Echo_Server_Network)
* 목적:
*
* 운영체제 : window11 와 window10
* 개발 IDE : visual studio
* 컴파일러 : visual studio compiler
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

#pragma comment(lib, "ws2_32.lib")

void get_ip(SOCKADDR_IN* addr) {
	printf(" ip : %s\n", "127.0.0.1");
	addr->sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(INADDR_ANY);
}

void get_port(SOCKADDR_IN* addr) {
	u_short port = 3000;

	printf(" port : %d\n", port);
	addr->sin_port = htons(port);
}

int tcp_server() {
	WSADATA wsa_init;
	SOCKET server, client;
	SOCKADDR_IN server_addr, client_addr;

	int client_addr_size, len = 0, loop = 0, port = 2000;
	char type = 0, msg[1501] = { 0 };

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init)) {
		printf(" WSA init failed\n");
		return 0;
	}

	system("cls");
	printf(" Open TCP socket...  ");

	server = socket(PF_INET, SOCK_STREAM, 0);

	//소켓 생성 오류 확인
	if (server == INVALID_SOCKET) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	//bind() 및 오류확인
	server_addr.sin_family = AF_INET;
	get_ip(&server_addr);

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
	
	//listen() 및 오류 확인 - TCP서버는 listen()을 통해 연결 대기 필요
	if (listen(server, 5) == -1) {
		printf(" Socket listen failed.\n");

		closesocket(server);
		WSACleanup();
		return 0;
	}

	printf(" Back | esc\n\n");

	while (1) {
		if (_kbhit() && _getch() == 27) break;

		//accept() 및 오류 확인 - TCP서버는 accept()를 이용
		client_addr_size = sizeof(client_addr);
		client = accept(server, (struct sockaddr*)&client_addr, &client_addr_size);

		while (recv(client, msg, 1500, 0)) {
			
			if (len == 0) continue;

			printf(" received :  %s\n", msg);

			send(client, msg, len, 0);

			if (msg == "exit\n") {
				closesocket(server);
				WSACleanup();
				system("cls");
				return 1;
			}
		}
		
	}
	
	

	closesocket(server);
	WSACleanup();
	system("cls");
	return 1;
}

int udp_server() {
	WSADATA wsa_init;
	SOCKET server, client;
	SOCKADDR_IN server_addr, client_addr;

	int client_addr_size, len = 0, loop = 0, port = 2000;
	char type = 0, msg[1501] = { 0 };

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init)) {
		printf(" WSA init failed\n");
		return 0;
	}

	system("cls");
	printf(" Open TCP socket...  ");

	server = socket(AF_INET, SOCK_DGRAM, 0);

	//소켓 생성 오류 확인
	if (server == -1) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	//bind() 및 오류확인
	server_addr.sin_family = AF_INET;
	get_ip(&server_addr);

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

	//recvfrom을 통해 받은 메시지 바로 읽어들이기
	while (1) {
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

		recvfrom(server, msg, 1500, 0, (struct sockaddr*)&client_addr, &client_addr_size);

		printf(" received :  ");
		printf("%s\n", msg);

		sendto(server, msg, len, 0, (struct sockaddr*)&client_addr, client_addr_size);
	}

	closesocket(server);
	WSACleanup();
	system("cls");
	return 1;
}

int tcp_client() {
	WSADATA wsa_init;
	SOCKET client;
	SOCKADDR_IN server_addr;

	int server_addr_size, loop = 0, len = 0, server_port = 1025;
	char type = 0, msg[1501] = { 0 }, server_ip[117] = { 0 };
	system("cls");

	if (WSAStartup(MAKEWORD(2, 2), &wsa_init) != 0) {
		printf(" WSA init failed.\n");
		return 0;
	}

	printf(" Open TCP socket...  ");

	client = socket(AF_INET, SOCK_STREAM, 0);

	//소켓 생성 오류 확인
	if (client == -1) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	printf(" Enter server ip : ");
	scanf("%s", server_ip);
	printf(" Enter server port : ");
	scanf("%d", &server_port);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons((u_short)server_port);

	if (connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		printf(" Server connect failed.\n");

		WSACleanup();
		return 0;
	}

	printf(" send :  ");
	//fgets(msg, 1500, stdin);
	scanf("%s", msg);

	send(client, msg, strlen(msg), 0);

	printf("sended\n");

	len = recv(client, msg, 1500, 0);
	
	printf("received\n");

	if (len > 0) {
		printf(" received :  ");
		do {
			printf("%s", msg);

			send(client, msg, len, 0);

			len = recv(client, msg, 1500, 0);
		} while (len > 0);
		printf("\n\n");
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

	int server_addr_size, loop = 0, len = 0, server_port = 1025;
	char type = 0, msg[1501] = { 0 }, server_ip[117] = { 0 };
	system("cls");

	printf(" Open UDP socket...  ");

	client = socket(AF_INET, SOCK_DGRAM, 0);

	//소켓 생성 오류 확인
	if (client == -1) {
		printf("failed.\n");
		WSACleanup();
		return 0;
	}
	printf("success.\n");

	printf("Enter server ip : ");
	scanf("%s", server_ip);
	printf("Enter server port : ");
	scanf("%d", &server_port);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons((u_short)server_port);

	printf(" send :  ");
	fgets(msg, 1500, stdin);

	sendto(client, msg, len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

	len = recvfrom(client, msg, 1500, 0, (struct sockaddr*)&server_addr, &server_addr_size);

	if (len > 0) {
		printf(" received :  ");
		do {
			printf("%s", msg);


			len = recvfrom(client, msg, 1500, 0, (struct sockaddr*)&server_addr, &server_addr_size);
		} while (len > 0);
		printf("\n\n");
	}

	closesocket(client);
	WSACleanup();
	system("cls");
	return 1;
}

int main() {
	int loop = 1;
	char sc_select = 0, type = 0;

	//본인이 서버인지 클라이언트인지 선택
	do {
		printf(" Are you a server or a client?\n\n server | S or s\n client | C or c\n End program | esc\n\n ENTER: ");
		sc_select = _getch();
		fflush(stdin);

		if (sc_select == 's' || sc_select == 'S') {
			system("cls");
			
			//TCP 방식으로 통신할지 UDP 방식으로 통신할지 선택
			do {
				printf(" Use TCP or UDP\n\n TCP | T or t\n UDP | U or u\n Back | esc\n\n Enter: ");
				type = _getch();
				fflush(stdin);

				//TCP 서버 생성
				if (type == 't' || type == 'T') loop = tcp_server();
				//UDP 서버 생성
				else if (type == 'u' || type == 'U') loop = udp_server();
				//뒤로 가기
				else if (type == ESC) {
					system("cls");
					break;
				}
				//잘못된 입력
				else {
					system("cls");
					printf(" WRONG INPUT OCCURED\n");
				}
			} while (loop);
		}
		else if (sc_select == 'c' || sc_select == 'C') {
			system("cls");

			//TCP 방식으로 통신할지 UDP 방식으로 통신할지 선택
			do {
				printf(" Use TCP or UDP\n\n TCP | T or t\n UDP | U or u\n Back | esc\n\n Enter: ");
				type = _getch();
				fflush(stdin);

				//TCP 소켓 생성
				if (type == 't' || type == 'T') loop = tcp_client();
				//UDP 소켓 생성
				else if (type == 'u' || type == 'U') loop = udp_client();
				//뒤로가기
				else if (type == ESC) {
					system("cls");
					break;
				}
				//잘못된 입력
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