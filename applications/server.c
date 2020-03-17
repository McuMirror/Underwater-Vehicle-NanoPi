/*
 * @Description: 服务器线程
 * @Author: chenxi
 * @Date: 2020-02-15 19:57:26
 * @LastEditTime: 2020-03-18 00:06:49
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

#define LISTEN_PORT 8888 

static int socket_fd = -1;
static int client_fd = -1;

uint8 return_data[RETURN_DATA_LEN] = {0xaa, 0x55, 0x16};//包头位固定数据

void print_hex_data(char *name, uint8 *data, int len)
{
    printf("%s:", name);
    for(int i = 0; i < len; i++){
        printf("%2x ", data[i]);
    }
    printf("\n");
}

void *send_thread(void *arg)
{
    while (1)
    {
        convert_return_computer_data(return_data);

        if (write(client_fd, return_data, RETURN_DATA_LEN) == -1){
            if (client_fd != -1){
                log_i("client closed");
                close(client_fd);
                client_fd = -1;
            }
            return NULL;
        }
        print_hex_data("send", return_data, RETURN_DATA_LEN);
        delay(1000);
    }
    return NULL;
}

void *recv_thread(void *arg)
{

    while (1)
    {
        if (recv(client_fd, recv_buff, RECE_DATA_LEN, 0) == -1)
        {
            if (client_fd != -1)
            {
                log_i("client closed");
                close(client_fd);
                client_fd = -1;
            }
            return NULL;
        }

        print_hex_data("recv", recv_buff, RECE_DATA_LEN);

        Remote_Control_Data_Analysis(recv_buff);
    }

    return NULL;
}

void *server_thread(void *arg)
{
    static struct sockaddr_in serverAddr;
    static struct sockaddr_in clientAddr;//用于保存客户端的地址信息
    static unsigned int AddrLen;
	static int iClientNum;//记录客户端连接的次数

    pthread_t send_tid;
    pthread_t recv_tid;
    
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        log_e("create server socket error:%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }
    log_d("socket_fd: %d", socket_fd);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //auto get local server ip addr
    serverAddr.sin_port = htons(LISTEN_PORT);

    if (bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
        log_e("bind socket error:%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(socket_fd, 10) == -1){
        log_e("listen socket error :%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }

    log_i("waiting for client to connect, server port: [%d]", LISTEN_PORT);
    while (1)
    {
        AddrLen = sizeof(struct sockaddr);
        if ((client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, &AddrLen)) == -1){
            log_e("accept socket error:%s(errorno:%d)", strerror(errno), errno);
            continue;
        }
        iClientNum++;
        log_d("client connected: %d", client_fd);
        log_i("get connet from clinet [NO.%d] : [%s]", iClientNum, inet_ntoa(clientAddr.sin_addr));//打印客户端连接次数及IP地址


        if (pthread_create(&send_tid, NULL, send_thread, NULL) == -1){
            log_e("send_thread create error!");
            return NULL;
        }
        if (pthread_detach(send_tid)){
            log_w("send_thread detach failed...");
            return NULL;
        }

        if (pthread_create(&recv_tid, NULL, recv_thread, NULL) == -1){
            log_e("recv_thread create error!");
            return NULL;
        }
        if (pthread_detach(recv_tid)){
            log_w("recv_thread detach failed...");
            return NULL;
        }
    }

    close(socket_fd);
}

int server_thread_init(void)
{
    pthread_t server_tid;
    if (pthread_create(&server_tid, NULL, server_thread, NULL) == -1){
        log_e("server_thread create error!");
        return 1;
    }

    if (pthread_detach(server_tid)){
        log_w("server_thread detach failed...");
        return -2;
    }
    return 0;
}