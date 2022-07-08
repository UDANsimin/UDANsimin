#include "hidef.h"
#include "derivative.h"
typedef struct  _Can_Msg    //定义发送报文的结构体
{
    unsigned int id;   //ID号
    unsigned char RTR;         //远程帧或数据帧标志位
    unsigned char data[8];  //存放数据
    unsigned char len;      //数据长度
    unsigned char prty;     //优先级
}Can_Msg;
static Can_Msg Can1_GetType = 
{
    0,
    0,
    {0,0,0,0,0,0,0,0},
    0,
    0
};

void INIT_PLL(void);
void INIT_CAN1(void);
int CAN1_Send(struct Can_Msg msg);
int CAN1_Receive(struct Can_Msg *msg1);
void CAN1_Call_Back(void);
void Initialize_Ect(void);