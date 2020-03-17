/*
 * @Description: 服务器线程
 * @Author: chenxi
 * @Date: 2020-02-15 19:57:26
 * @LastEditTime: 2020-03-17 23:29:57
 * @LastEditors: chenxi
 */

#define LOG_TAG "server"

#include "../easylogger/inc/elog.h"
#include "server.h"
#include "sensor.h"
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
#include <pthread.h>

#include <wiringPi.h>

#define LISTEN_PORT 8000

static int socket_fd = -1, client_fd = -1;
extern Sensor_Type Sensor;
extern uint8 begin_buff[3];
extern uint8 Return_Data[Send_Date_Len];

int Send_Buffer_Agreement(int fd, uint8 *begin_buff, uint8 *buff, uint8 len)
{
    uint8 Check_Byte = 0;

    Check_Byte = Calculate_Check_Byte(begin_buff, buff, len); //计算校验位

    if (write(client_fd, begin_buff, 3) == -1)
    {
        if (client_fd != -1)
        {
            log_i("client closed");
            close(client_fd);
            client_fd = -1;
        }
        return -1;
    }

    write(client_fd, buff, len);      //发送数据包
    write(client_fd, &Check_Byte, 1); //发送校验位
    return 0;
}

void *send_thread(void *arg)
{
    while (1)
    {
        Convert_Return_Computer_Data(Return_Data);
        if (Send_Buffer_Agreement(client_fd, begin_buff, Return_Data, Send_Date_Len) == -1)
        {
            return NULL;
        }
        delay(1000);

        printf("send to PC: ");
        for (int i = 0; i < Send_Date_Len; i++)
            printf("%4d", Return_Data[i]);
        printf("\n");
    }
    return NULL;
}

void *recv_thread(void *arg)
{
    uint8 recv_buff[Recv_Date_Len] = {0};
    while (1)
    {
        if (recv(client_fd, recv_buff, Recv_Date_Len, 0) == -1)
        {
            if (client_fd != -1)
            {
                log_i("client closed");
                close(client_fd);
                client_fd = -1;
            }
            return NULL;
        }

        printf("recv from PC: ");
        for (int i = 0; i < Recv_Date_Len; i++)
            printf("%3X", recv_buff[i]);
        printf("\n");

        Remote_Control_Data_Analysis(recv_buff);
    }

    return NULL;
}

void *server_thread(void *arg)
{
    struct sockaddr_in serverAddr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {

        log_e("create server socket error:%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }
    log_d("socket_fd: %d", socket_fd);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //auto get local server ip addr
    serverAddr.sin_port = htons(LISTEN_PORT);

    if (bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        log_e("bind socket error:%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(socket_fd, 10) == -1)
    {
        log_e("listen socket error :%s(errno:%d)\n", strerror(errno), errno);
        exit(0);
    }

    log_i("waiting for client to connect");
    while (1)
    {
        if ((client_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL)) == -1)
        {
            log_e("accept socket error:%s(errorno:%d)", strerror(errno), errno);
            continue;
        }
        log_d("client connected: %d", client_fd);

        pthread_t send_tid;
        if (pthread_create(&send_tid, NULL, send_thread, NULL) == -1)
        {
            log_e("send_thread create error!");
            return NULL;
        }
        if (pthread_detach(send_tid))
        {
            log_w("send_thread detach failed...");
            return NULL;
        }

        pthread_t recv_tid;
        if (pthread_create(&recv_tid, NULL, recv_thread, NULL) == -1)
        {
            log_e("recv_thread create error!");
            return NULL;
        }
        if (pthread_detach(recv_tid))
        {
            log_w("recv_thread detach failed...");
            return NULL;
        }
    }

    close(socket_fd);
}

int server_thread_init(void)
{
    pthread_t server_tid;
    if (pthread_create(&server_tid, NULL, server_thread, NULL) == -1)
    {
        log_e("server_thread create error!");
        return 1;
    }
    if (pthread_detach(server_tid))
    {
        log_w("server_thread detach failed...");
        return -2;
    }
    return 0;
}