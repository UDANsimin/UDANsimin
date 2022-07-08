#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "can.h"
//#define ID                  0x0001
//#define ID1                  0x0002    //发送标识符ID号
//#define data_len_TX          8           //发送数据长度
#define MODE_ONE PORTC_PC4
#define MODE_ONE_dir DDRC_DDRC4
#define MODE_TWO PORTC_PC5
#define MODE_TWO_dir DDRC_DDRC5      //上电
void main(void) {
  DisableInterrupts;           //禁止打开所有中断
  INIT_PLL();
  INIT_CAN1();
  Initialize_Ect();                 //初始化can0模块
  MODE_ONE=1;                           //上电
  MODE_ONE_dir=1;
  MODE_TWO=0;
  MODE_TWO_dir=1;
  EnableInterrupts;            //允许打开所有中断
  for(;;)
  {
     _FEED_COP();   /* feeds the dog */
  }
}