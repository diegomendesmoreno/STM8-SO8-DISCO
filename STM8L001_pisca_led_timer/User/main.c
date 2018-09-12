/* Projeto    : STM8L001_pisca_led_timer
 * MCU        : STM8L001J3M3TR
 * Copyright  : Atribuição (CC BY 4.0) Diego Moreno
 * Versão     : 1.0.0
 * Data	      : 20/08/2018
 * Descrição  : Projeto template para STM8L001J3
 */

/* Includes */
#include "stm8l10x.h"

/* Defines de usuário */
#ifdef _COSMIC_
  #define ASM  _asm
#endif
#ifdef EWSTM8_IAR
  #define ASM  asm
#endif

/* This delay should be added just after reset to have access to SWIM pin and to be
   able to reprogram the device after power on (otherwise the device will be locked) */
#define STARTUP_SWIM_DELAY_5S      \
{                                  \
  ASM("       PUSHW X        \n"   \
      "       PUSH  A        \n"   \
      "       LDW X, #0xFFFF \n"   \
      "loop1: LD  A, #50     \n"   \
                                   \
      "loop2: DEC A          \n"   \
      "       JRNE loop2     \n"   \
                                   \
      "       DECW X         \n"   \
      "       JRNE loop1     \n"   \
                                   \
      "       POP  A         \n"   \
      "       POPW X           "); \
}

/* Declaração de Subrotinas */
static void TIM2_Config(void);
void delay(unsigned int milissegundos);

/* Variáveis globais */
volatile unsigned int u16_cnt_ms = 0;
volatile unsigned char u8_UserSW = 0;

main()
{
  /* delay for SWIM connection: ~5seconds */
//  STARTUP_SWIM_DELAY_5S;
	
  //Configuração de Clock
  CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);  //Clock interno = 16MHz
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  
  /* Inicializações */
  /* -- Pin 1 --------------------------------------------------------------- */
//  GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PA0/SWIM/BEEP/IR_TIM
//  GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast); //PC3/USART_TX
//  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //PC4/USART_CK/CCO
  /* -- Pin 2 --------------------------------------------------------------- */
//  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PA2
//  GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //PA4/TIM2_BKIN
//  GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PA6/COMP_REF
  /* -- Pin 5 --------------------------------------------------------------- */
  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);   //PB3/TIM2_ETR/COMP2_CH2 <<<<==== LED
//  GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast); //PB5/SPI_SCK
//  GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PD0/TIM3_CH2/COMP1_CH3
  /* -- Pin 6 --------------------------------------------------------------- */
//  GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast); //PB6/SPI_MOSI
  /* -- Pin 7 --------------------------------------------------------------- */
  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_IT);          //PB7/SPI_MISO <<<<==== USER_SW
//  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //PC0/I2C_SDA
  /* -- Pin 8 --------------------------------------------------------------- */
//  GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast); //PC1/I2C_SCL
//  GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PC2/USART_RX
  /* ------------------------------------------------------------------------ */
  
  EXTI_SetPinSensitivity(EXTI_Pin_7, EXTI_Trigger_Rising);  //Interrução externa Pino7
  TIM2_Config();
  
  //Permitir interrupções
  enableInterrupts();
  
  while(1)
  {
    switch(u8_UserSW)
    {
      case 0 :
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, SET);
        delay(100);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, RESET);
        delay(100);
        break;
      
      case 1 :
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, SET);
        delay(250);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, RESET);
        delay(250);
        break;
      
      case 2 :
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, SET);
        delay(500);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, RESET);
        delay(500);
        break;
      
      case 3 :
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, SET);
        delay(1000);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, RESET);
        delay(1000);
        break;
      
      default :
        break;
    }
  }
}

/* Subrotina  : TIM2_Config
 * Descrição  : Inicializa o timer TIM2.
 * Parâmetros : void
 * Retorno    : void
 */
static void TIM2_Config(void)
{
  //Configuração para frequência TIM2 = 1000 Hz
  uint16_t Period = 999;
  
  //Inicialização do TIM2
  TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, Period);
  
  //Escolhe interrupção por OVERFLOW ("UPDATE")
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
  
  //Ativa o periférico TIM2
  TIM2_Cmd(ENABLE);
  
  //Gera um evento para ativar as configurações
  TIM2->EGR |= 0x0001;
  
  //Limpa o flag do TIM2
  TIM2_ClearFlag(TIM2_FLAG_Update);
}

/* Subrotina  : delay
 * Descrição  : Cria um atraso com o tempo em milisegundos predeterminado.
 * Parâmetros : Tempo de atraso em milissegundos
 * Retorno    : void
 */
void delay(unsigned int milissegundos)
{
  u16_cnt_ms = 0;
  
  while(u16_cnt_ms < milissegundos);
}