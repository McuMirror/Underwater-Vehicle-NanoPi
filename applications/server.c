/*
 * @Description: 服务器线程
 * @Author: chenxi
 * @Date: 2020-02-15 19:57:26
 * @LastEditTime: 2020-03-18 19:17:09
 * @LastEditors: chenxi
 */

#define LOG_TAG "server"

#include "../easylogger/inc/elog.h"
#include "server.h"
#include "ret_data.h"
#include "rc_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <wiringPi.h>

#define LISTEN_PORT 8899 
#define BACKLOG     10   //最大连接数

static int sever_sock = -1;
static int client_sock = -1;

//包头位固定为：0xaa,0x55;  数据长度位：0x16
uint8 return_data[RETURN_DATA_LEN] = {0xaa, 0x55, 0x16};

void print_hex_data(char *name, uint8 *data, int len)
{
    printf("%s:", name);
    for(int i = 0; i < len; i++)
    {
        printf("%2x ", data[i]);
    }
    printf("\n");
}

void *send_thread(void *arg)
{
    while (1)
    {
        convert_rov_data(return_data);//转换rov数据

        if (write(client_sock, return_data, RETURN_DATA_LEN) < 0)
        {
            if (client_sock != -1)
            {
                log_i("client closed");
                close(client_sock);
                client_sock = -1;
            }
            return NULL;
        }
        print_hex_data("send", return_data, RETURN_DATA_LEN);
        sleep(1);//1s更新一次
    }
    return NULL;
}

void *recv_thread(void *arg)
{

    while (1)
    {
        if (recv(client_sock, recv_buff, RECE_DATA_LEN, 0) < 0)
        {
            if (client_sock != -1)
            {
                log_i("client closed");
                close(client_sock);
                client_sock = -1;
            }
            return NULL;
        }
        print_hex_data("recv", recv_buff, RECE_DATA_LEN);
        //遥控数据解析
        remote_control_data_analysis(recv_buff);
    }

    return NULL;
}

void *server_thread(void *arg)
{
    static struct sockaddr_in serverAddr;
    static struct sockaddr_in clientAddr; // 用于保存客户端的地址信息
    static unsigned int addrLen;
	static int clientCnt; // 记录客户端连接的次数
    static int on, ret;

    pthread_t send_tid;
    pthread_t recv_tid;
    
    if ((sever_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        log_e("create server socket error:%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }
    log_d("sever_sock: %d", sever_sock);
    /* Enable address reuse */
    on = 1; 
    // SO_REUSERADDR 允许重用本地地址和端口，充许绑定已被使用的地址（或端口号）
    if ((ret = setsockopt(sever_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        log_e("setsockopt port for reuse error:%s(errno:%d)\n", strerror(errno), errno);
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //auto get local server ip addr
    serverAddr.sin_port = htons(LISTEN_PORT);

    if (bind(sever_sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        log_e("bind socket error:%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(sever_sock, BACKLOG) < 0){
        log_e("listen socket error :%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }

    log_i("waiting for client to connect, server port: [%d]", LISTEN_PORT);
    while (1)
    {
        addrLen = sizeof(struct sockaddr);
        if ((client_sock = accept(sever_sock, (struct sockaddr *)&clientAddr, &addrLen)) < 0)
        {
            log_e("accept socket error:%s(errorno:%d)", strerror(errno), errno);
            continue;
        }
        clientCnt++;
        log_d("client connected: %d", client_sock);
        //打印客户端连接次数及IP地址
        log_i("conneted success from clinet [NO.%d] : [%s]", clientCnt, inet_ntoa(clientAddr.sin_addr));
        if (pthread_create(&send_tid, NULL, send_thread, NULL) < 0)
        {
            log_e("send_thread create error!");
            return NULL;
        }
        if (pthread_detach(send_tid))
        {
            log_w("send_thread detach failed...");
            return NULL;
        }

        if (pthread_create(&recv_tid, NULL, recv_thread, NULL) < 0)
        {
            log_e("recv_thread create error!");
            return NULL;
        }
        if (pthread_detach(recv_tid)){
            log_w("recv_thread detach failed...");
            return NULL;
        }
    }

    close(sever_sock);
}
pthread_t server_tid;
int server_thread_init(void)
{

    if (pthread_create(&server_tid, NULL, server_thread, NULL) < 0){
        log_e("server_thread create error!");
        return 1;
    }

    if (pthread_detach(server_tid)){
        log_w("server_thread detach failed...");
        return -2;
    }
    return 0;
}