#include "hidef.h"
#include "derivative.h"
typedef struct  _Can_Msg    //���巢�ͱ��ĵĽṹ��
{
    unsigned int id;   //ID��
    unsigned char RTR;         //Զ��֡������֡��־λ
    unsigned char data[8];  //�������
    unsigned char len;      //���ݳ���
    unsigned char prty;     //���ȼ�
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