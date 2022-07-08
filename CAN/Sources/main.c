#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "can.h"
//#define ID                  0x0001
//#define ID1                  0x0002    //���ͱ�ʶ��ID��
//#define data_len_TX          8           //�������ݳ���
#define MODE_ONE PORTC_PC4
#define MODE_ONE_dir DDRC_DDRC4
#define MODE_TWO PORTC_PC5
#define MODE_TWO_dir DDRC_DDRC5      //�ϵ�
void main(void) {
  DisableInterrupts;           //��ֹ�������ж�
  INIT_PLL();
  INIT_CAN1();
  Initialize_Ect();                 //��ʼ��can0ģ��
  MODE_ONE=1;                           //�ϵ�
  MODE_ONE_dir=1;
  MODE_TWO=0;
  MODE_TWO_dir=1;
  EnableInterrupts;            //����������ж�
  for(;;)
  {
     _FEED_COP();   /* feeds the dog */
  }
}