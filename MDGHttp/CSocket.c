#include "stdafx.h"

///������
WSADATA CSocket_wsa_data;
SOCKET  CSocket_srv_soc = 0, CSocket_acpt_soc;
struct sockaddr_in CSocket_serv_addr;   
struct sockaddr_in CSocket_from_addr;   
char CSocket_recv_buf[HTTP_BUF_SIZE];
unsigned short CSocket_port = HTTP_DEF_PORT;
int from_len = sizeof(CSocket_from_addr);
int result = 0, recv_len;
struct http_st_HttpReq CSocket_HttpReq;

/* �����ļ����Ͷ�Ӧ�� Content-Type */
struct doc_type
{
	char *suffix; /* �ļ���׺ */
	char *type;   /* Content-Type */
};

//**mime ����**/
struct doc_type file_type[] =
{
	{ "html",    "text/html" },
	{ "gif",     "image/gif" },
	{ "jpeg",    "image/jpeg" },
	{ NULL,      NULL }
};

int CSocket_fnInitSocket()
{
	WSAStartup(MAKEWORD(2, 0), &CSocket_wsa_data); /* ��ʼ�� WinSock ��Դ */

	CSocket_srv_soc = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
	if (CSocket_srv_soc == INVALID_SOCKET)
	{
		printf("[Web] WSAStartup error = %d\n", WSAGetLastError());
		return -1;
	}

	/* ��������ַ */
	CSocket_serv_addr.sin_family = AF_INET;
	CSocket_serv_addr.sin_port = htons(CSocket_port);
	CSocket_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(CSocket_srv_soc, (struct sockaddr *) &CSocket_serv_addr, sizeof(CSocket_serv_addr));
	if (result == SOCKET_ERROR) /* ��ʧ�� */
	{
		closesocket(CSocket_srv_soc);
		printf("[Web] bind error = %d\n", WSAGetLastError());
		return -1;
	}

	result = listen(CSocket_srv_soc, SOMAXCONN);
	printf("[Web] running ... ...\n");
	return 0;
}

int CSocket_fnAcceptSocket() 
{
	//�ȴ�����
	CSocket_acpt_soc = accept(CSocket_srv_soc, (struct sockaddr *) &CSocket_from_addr, &from_len);
	if (CSocket_acpt_soc == INVALID_SOCKET) 
	{
		printf("[Web] ����ʧ�� �� %d\n", WSAGetLastError());
		return -1;
	}

	printf("[Web] ���ӳɹ�:[%s], �˿�:[%d]\n",
		inet_ntoa(CSocket_from_addr.sin_addr), ntohs(CSocket_from_addr.sin_port));
	return 0;
}

int CSocket_fnRecvSocket()
{
	CSocket_HttpReq = http_fnGetHeaders(CSocket_acpt_soc);
	//http_fnGetHeaders(CSocket_acpt_soc, &CSocket_HttpReq);
	
	/* ��ͻ��˷�����Ӧ���� */
	result = CSocket_FnSend_response(CSocket_acpt_soc, &httpheader, 1024);
	closesocket(CSocket_acpt_soc);
	return 0;
}

void CSocket_fnClose() 
{
	closesocket(CSocket_srv_soc);
	WSACleanup();
	printf("[Web] stopped.\n");
}

///������Ӧ����
int CSocket_FnSend_response(SOCKET soc, char *buf, int buf_len)
{
	// ���� HTTP ��
	int i = 0;
	i+= http_fnSendHeaders(soc);
	//
	i+= http_fnSendContent(soc,buf, buf_len);
	if (1 != 0)
	{
		//error 
		//TODO
	}
	return 1;
}


int get_line(int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';
	int n;

	/*����ֹ����ͳһΪ \n ���з�����׼�� buf ����*/
	while ((i < size - 1) && (c != '\n'))
	{
		n = recv(sock, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\r')
			{
				n = recv(sock, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';
	return(i);
}
