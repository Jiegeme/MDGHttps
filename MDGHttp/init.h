#pragma once


int init();

//
char httpheader[1024];

/*����ͷ������*/
struct http_st_HttpReq
{
	char *header;
	char *url;
	char *get;
	char *post;
	char *type;
	int http_state;
};
