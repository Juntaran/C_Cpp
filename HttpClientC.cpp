// 用C代码模拟浏览器IE(http client)访问web(http server)的行为

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	// 网络初始化
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1,1), &wsaData);

	// 我们来访问一下度娘
	char szWeb[] = "www.baidu.com"; // !!!强调: 你的pc一定要能ping通www.baidu.com, 否则程序不能正常运行就不要找我啦
    HOSTENT *pHost = gethostbyname(szWeb);
    
	// 度娘的ip地址
	const char* pIPAddr = inet_ntoa(*((struct in_addr *)pHost->h_addr)) ;
    printf("web server ip is : %s\n", pIPAddr);

	// 度娘服务器信息
    SOCKADDR_IN  webServerAddr;
    webServerAddr.sin_family = AF_INET;
    webServerAddr.sin_addr.S_un.S_addr=inet_addr(pIPAddr);
    webServerAddr.sin_port = htons(80);
 
	// 创建客户端通信socket
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	// 去连接度娘
    int nRet = connect(sockClient ,(struct sockaddr*)&webServerAddr, sizeof(webServerAddr));
    if(nRet < 0)
	{
		printf("connect error\n");
		return 1;
	}

	// 准备向度娘发送http的GET请求
    char szHttpRest[1024] = {0};
	sprintf(szHttpRest, "GET /index.html HTTP/1.1\r\nHost:%s\r\nConnection: Keep-Alive\r\n\r\n", szWeb);
    printf("\n---------------------send is :---------------------\n");
	printf("%s\n", szHttpRest);
 
	// 利用tcp向度娘发送http的GET请求
    nRet = send(sockClient , szHttpRest, strlen(szHttpRest) + 1, 0);
	if(nRet < 0)
	{
		printf("send error\n");
		return 1;
	}

	// 把度娘返回的信息保存在文件test.txt中
	FILE *fp = fopen("test.txt", "w");
	while(1)
	{
			// 每次接收一个字节
		    char szRecvBuf[2] = {0};
			nRet = recv(sockClient ,szRecvBuf, 1 ,0);
			
			// 接收错误
			if(nRet < 0)
			{
				printf("recv error\n");
				goto LABEL;
			}			
			
			// 度娘主动断开了连接
			if(0 == nRet)
			{
				printf("connection has beed closed by web server\n");
				goto LABEL;
			}
			
			static int flag = 0;
			if(0 == flag)
			{
				printf("writing data to file\n");
				flag = 1;
			}

			// 把度娘返回的信息写入文件
			fputc(szRecvBuf[0], fp);
	}


LABEL: // 这个单词不要写错啦， 很多童鞋容易写错
	fclose(fp);
	closesocket(sockClient);  
    WSACleanup();  

    return nRet < 0 ? 1 : 0;
}