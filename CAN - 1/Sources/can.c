#include "can.h"
/*****************************************************************************/
/*                             初始化锁相环                                                   */
/*****************************************************************************/
void INIT_PLL(void)
{
    CLKSEL_PLLSEL=0;
    SYNR=0x47;
    REFDV=0x41;
    POSTDIV=0x00;
    PLLCTL_PLLON=1;
    _asm(nop);
    _asm(nop);
    while(CRGFLG_LOCK==0)
    {
    }
    CLKSEL_PLLSEL =1;
}
/*****************************************************************************/
/*                             初始化CAN1                                                     */
/*****************************************************************************/
void INIT_CAN1(void)
{
    if(CAN1CTL0_INITRQ==0)            //查询是否进入初始化状态
      CAN1CTL0_INITRQ =1;              //进入初始化状态
    while (CAN1CTL1_INITAK==0);     //等待进入初始化状态
    CAN1BTR0_SJW = 0X00;             //设置同步
    CAN1BTR0_BRP =  0X0E;            //预分频因子为16
    CAN1BTR1 = 0X1D;                     //设置TSEG1为14，TSEG2为2
    CAN1IDMR0 = 0xFF;                    //关闭滤波器
    CAN1IDMR1 = 0xFF;
    CAN1IDMR2 = 0xFF;
    CAN1IDMR3 = 0xFF;
    CAN1IDMR4 = 0xFF;
    CAN1IDMR5 = 0xFF;
    CAN1IDMR6 = 0xFF;
    CAN1IDMR7 = 0xFF;
    CAN1CTL1 = 0xC0;                        //使能MSCAN模块，MSCAN时钟源为can总线时钟
    CAN1CTL0 = 0x00;                        //返回正常运行模式
    while(CAN1CTL1_INITAK)              //等待回到一般运行模式
    {
    }
    while(CAN1CTL0_SYNCH == 1)     //等待总线时钟同步
    {
    }
    CAN1RIER_RXFIE = 1;                 //使能接收中断
}
/*****************************************************************************/
/*                             ECT模块                                                          */
/*****************************************************************************/
void Initialize_Ect(void)
{
    ECT_TSCR1_TFFCA = 1;  // 定时器标志位快速清除
    ECT_TSCR1_TEN = 1;    // 定时器使能位. 1=允许定时器正常工作; 0=使主定时器不起作用(包括计数器)
    ECT_TIOS  = 0xff;     //指定所有通道为输出比较方式
    ECT_TCTL1 = 0x00;   // 后四个通道设置为定时器与输出引脚断开
    ECT_TCTL2 = 0x00;     // 前四个通道设置为定时器与输出引脚断开
    ECT_DLYCT = 0x00;   // 延迟控制功能禁止
    ECT_ICOVW = 0x00;   // 对应的寄存器允许被覆盖;  NOVWx = 1, 对应的寄存器不允许覆盖
    ECT_ICSYS = 0x00;   // 禁止IC及PAC的保持寄存器
    ECT_TIE   = 0x00;     // 禁止所有通道定时中断
    ECT_TSCR2 = 0x07;   // 预分频系数pr2-pr0:111,,时钟周期为4us,
    ECT_TFLG1 = 0xff;   // 清除各IC/OC中断标志位
    ECT_TFLG2 = 0xff;     // 清除自由定时器中断标志位
}
/*****************************************************************************/
/*                             CAN发送                                                          */
/*****************************************************************************/
int CAN1_Send (Can_Msg msg)     //CAN发送
{
    unsigned char send_buf, sp;
    if(msg.len > 8)
      return(FALSE);    // 检查数据长度
    if(CAN1CTL0_SYNCH==0)
      return(FALSE);        // 检查总线时钟是否同步
    send_buf = 0;
    do
    {
        CAN1TBSEL=CAN1TFLG;
        send_buf=CAN1TBSEL;
    }
  while(!send_buf);  // 寻找空闲的缓冲器
    CAN1TXIDR0 = (unsigned char)(msg.id>>3);
    CAN1TXIDR1 = (unsigned char)(msg.id<<5);   // 写入标识符
    if(msg.RTR)
      CAN1TXIDR3 |= 0X10;    //检测数据帧或标准帧
    for(sp = 0; sp < msg.len; sp++)
      *((&CAN1TXDSR0)+sp) = msg.data[sp];   // 写入数据
    CAN1TXDLR = msg.len;        // 写入数据长度
    CAN1TXTBPR = msg.prty;   // 写入优先级
    CAN1TFLG = send_buf;     // 清 TXx 标志 (缓冲器准备发送)
    return(TRUE);
}
/*************************************************************/
/*                       CAN接收                                          */
/*************************************************************/
int CAN1_Receive(Can_Msg *msg1)
{
  unsigned char sp2;
  // 检测接收标志
  if(!(CAN1RFLG_RXF))
    return(FALSE);
  // 检测 CAN协议报文模式 （一般/扩展） 标识符
  if(CAN1RXIDR1_IDE)  //0为标准格式，1为扩展格式
    // IDE = Recessive (Extended Mode)
    return(FALSE);
  // 读标识符
  msg1->id = (unsigned int)(CAN1RXIDR0<<3) |
            (unsigned char)(CAN1RXIDR1>>5);
  if(CAN1RXIDR1&0x10)
    msg1->RTR = TRUE;
  else
    msg1->RTR = FALSE;
  // 读取数据长度
  msg1->len = CAN1RXDLR;
  // 读取数据
  for(sp2 = 0; sp2 < msg1->len; sp2++)
    msg1->data[sp2] = *((&CAN0RXDSR0)+sp2);
  // 清 RXF 标志位 (缓冲器准备接收)
  CAN1RFLG = 0x01;
  return(TRUE);
}
/*************************************************************/
/*                       返回函数                                          */
/*************************************************************/
void CAN1_Call_Back()
{
    Can_Msg CAN_MSG =
    {
        0x00000000,
        0,
        {1,1,1,1,1,1,1,1},
        8,
        0
    };
    if (CAN1_Receive(&Can1_GetType) == TRUE)
    {
      CAN1_Send(CAN_MSG);
     if (Can1_GetType.id == 0x01 && (!Can1_GetType.RTR))
        {
            CAN1_Send(CAN_MSG);
        }
    }
}
