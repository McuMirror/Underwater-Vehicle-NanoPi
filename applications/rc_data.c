/*
 * @Description: 处理接收数据
 * @Author: chenxi
 * @Date: 2020-02-17 15:13:19
 * @LastEditTime: 2020-02-17 15:13:20
 * @LastEditors: chenxi
 */

#define LOG_TAG "rc_data"

#include "../easylogger/inc/elog.h"
#include "rc_data.h"

ReceiveData_Type ReceiveData = {
    .THR = 1500,
    .YAW = 1500,
    .ROL = 1500,
    .PIT = 1500};

ControlCmd_Type ControlCmd = {
    //上位机控制数据
    .Power = 0,
    .All_Lock = LOCK //初始化定义为锁定状态
};

Rocker_Type Rocker; // 摇杆数据结构体

uint8 RC_Control_Data[RECE_DATA_LEN] = {0};
uint8 Control_RxCheck = 0; //尾校验字
uint8 recv_buff[RECE_DATA_LEN] = {0};
/**
  * @brief  remote_control_data_analysis(控制数据解析)
  * @param  控制字符数据 uint8 *recv_buff
  * @retval None
  * @notice 从第四个字节开始为控制字符
  */
void remote_control_data_analysis(uint8 *recv_buff) //控制数据解析
{
    if (recv_buff[0] == 0xAA && recv_buff[1] == 0x55 && recv_buff[2] == 0x10) //检测包头
    {
        for (int i = 0; i < RECE_DATA_LEN - 1; i++)
        {
            Control_RxCheck += recv_buff[i];
        }
        if (Control_RxCheck == recv_buff[RECE_DATA_LEN]) //校验
        {
            ControlCmd.Depth_Lock = RC_Control_Data[3];     //深度锁定
            ControlCmd.Direction_Lock = RC_Control_Data[4]; //方向锁定

            ControlCmd.Move = RC_Control_Data[5];        //前后
            ControlCmd.Translation = RC_Control_Data[6]; //左右平移
            ControlCmd.Vertical = RC_Control_Data[7];    //垂直
            ControlCmd.Rotate = RC_Control_Data[8];      //旋转

            ControlCmd.Power = RC_Control_Data[9];   //动力控制  推进器动力系数
            ControlCmd.Light = RC_Control_Data[10];  //灯光控制
            ControlCmd.Focus = RC_Control_Data[11];  //变焦摄像头控制
            ControlCmd.Yuntai = RC_Control_Data[12]; //云台控制
            ControlCmd.Arm = RC_Control_Data[13];    //机械臂控制
            // ControlCmd.Raspi = RC_Control_Data[14];  //树莓派启动位
            ControlCmd.All_Lock = RC_Control_Data[18];
        }
    }
}

/* 
【注意】这里仅清空控制数据指令，不能清除控制状态指令，因此，不能采用 meset 直接填充结构体为 0 
*/
void Control_Cmd_Clear(ControlCmd_Type *cmd)
{
    cmd->Move = 0;        //前后
    cmd->Translation = 0; //左右平移
    cmd->Vertical = 0;    //垂直
    cmd->Rotate = 0;      //旋转

    cmd->Light = 0;  //灯光控制
    cmd->Focus = 0;  //变焦摄像头控制
    cmd->Yuntai = 0; //云台控制
    cmd->Arm = 0;    //机械臂控制
}
