#include "can.h"
/*****************************************************************************/
/*                             ��ʼ�����໷                                                   */
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
/*                             ��ʼ��CAN1                                                     */
/*****************************************************************************/
void INIT_CAN1(void)
{
    if(CAN1CTL0_INITRQ==0)            //��ѯ�Ƿ�����ʼ��״̬
      CAN1CTL0_INITRQ =1;              //�����ʼ��״̬
    while (CAN1CTL1_INITAK==0);     //�ȴ������ʼ��״̬
    CAN1BTR0_SJW = 0X00;             //����ͬ��
    CAN1BTR0_BRP =  0X0E;            //Ԥ��Ƶ����Ϊ16
    CAN1BTR1 = 0X1D;                     //����TSEG1Ϊ14��TSEG2Ϊ2
    CAN1IDMR0 = 0xFF;                    //�ر��˲���
    CAN1IDMR1 = 0xFF;
    CAN1IDMR2 = 0xFF;
    CAN1IDMR3 = 0xFF;
    CAN1IDMR4 = 0xFF;
    CAN1IDMR5 = 0xFF;
    CAN1IDMR6 = 0xFF;
    CAN1IDMR7 = 0xFF;
    CAN1CTL1 = 0xC0;                        //ʹ��MSCANģ�飬MSCANʱ��ԴΪcan����ʱ��
    CAN1CTL0 = 0x00;                        //������������ģʽ
    while(CAN1CTL1_INITAK)              //�ȴ��ص�һ������ģʽ
    {
    }
    while(CAN1CTL0_SYNCH == 1)     //�ȴ�����ʱ��ͬ��
    {
    }
    CAN1RIER_RXFIE = 1;                 //ʹ�ܽ����ж�
}
/*****************************************************************************/
/*                             ECTģ��                                                          */
/*****************************************************************************/
void Initialize_Ect(void)
{
    ECT_TSCR1_TFFCA = 1;  // ��ʱ����־λ�������
    ECT_TSCR1_TEN = 1;    // ��ʱ��ʹ��λ. 1=����ʱ����������; 0=ʹ����ʱ����������(����������)
    ECT_TIOS  = 0xff;     //ָ������ͨ��Ϊ����ȽϷ�ʽ
    ECT_TCTL1 = 0x00;   // ���ĸ�ͨ������Ϊ��ʱ����������ŶϿ�
    ECT_TCTL2 = 0x00;     // ǰ�ĸ�ͨ������Ϊ��ʱ����������ŶϿ�
    ECT_DLYCT = 0x00;   // �ӳٿ��ƹ��ܽ�ֹ
    ECT_ICOVW = 0x00;   // ��Ӧ�ļĴ�����������;  NOVWx = 1, ��Ӧ�ļĴ�����������
    ECT_ICSYS = 0x00;   // ��ֹIC��PAC�ı��ּĴ���
    ECT_TIE   = 0x00;     // ��ֹ����ͨ����ʱ�ж�
    ECT_TSCR2 = 0x07;   // Ԥ��Ƶϵ��pr2-pr0:111,,ʱ������Ϊ4us,
    ECT_TFLG1 = 0xff;   // �����IC/OC�жϱ�־λ
    ECT_TFLG2 = 0xff;     // ������ɶ�ʱ���жϱ�־λ
}
/*****************************************************************************/
/*                             CAN����                                                          */
/*****************************************************************************/
int CAN1_Send (Can_Msg msg)     //CAN����
{
    unsigned char send_buf, sp;
    if(msg.len > 8)
      return(FALSE);    // ������ݳ���
    if(CAN1CTL0_SYNCH==0)
      return(FALSE);        // �������ʱ���Ƿ�ͬ��
    send_buf = 0;
    do
    {
        CAN1TBSEL=CAN1TFLG;
        send_buf=CAN1TBSEL;
    }
  while(!send_buf);  // Ѱ�ҿ��еĻ�����
    CAN1TXIDR0 = (unsigned char)(msg.id>>3);
    CAN1TXIDR1 = (unsigned char)(msg.id<<5);   // д���ʶ��
    if(msg.RTR)
      CAN1TXIDR3 |= 0X10;    //�������֡���׼֡
    for(sp = 0; sp < msg.len; sp++)
      *((&CAN1TXDSR0)+sp) = msg.data[sp];   // д������
    CAN1TXDLR = msg.len;        // д�����ݳ���
    CAN1TXTBPR = msg.prty;   // д�����ȼ�
    CAN1TFLG = send_buf;     // �� TXx ��־ (������׼������)
    return(TRUE);
}
/*************************************************************/
/*                       CAN����                                          */
/*************************************************************/
int CAN1_Receive(Can_Msg *msg1)
{
  unsigned char sp2;
  // �����ձ�־
  if(!(CAN1RFLG_RXF))
    return(FALSE);
  // ��� CANЭ�鱨��ģʽ ��һ��/��չ�� ��ʶ��
  if(CAN1RXIDR1_IDE)  //0Ϊ��׼��ʽ��1Ϊ��չ��ʽ
    // IDE = Recessive (Extended Mode)
    return(FALSE);
  // ����ʶ��
  msg1->id = (unsigned int)(CAN1RXIDR0<<3) |
            (unsigned char)(CAN1RXIDR1>>5);
  if(CAN1RXIDR1&0x10)
    msg1->RTR = TRUE;
  else
    msg1->RTR = FALSE;
  // ��ȡ���ݳ���
  msg1->len = CAN1RXDLR;
  // ��ȡ����
  for(sp2 = 0; sp2 < msg1->len; sp2++)
    msg1->data[sp2] = *((&CAN0RXDSR0)+sp2);
  // �� RXF ��־λ (������׼������)
  CAN1RFLG = 0x01;
  return(TRUE);
}
/*************************************************************/
/*                       ���غ���                                          */
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
