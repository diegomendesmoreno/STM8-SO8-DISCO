/* Projeto    : STM8S001_pisca_led_timer
 * MCU        : STM8S001J3M3TR
 * Copyright  : Atribuição (CC BY 4.0) Diego Moreno
 * Versão     : 1.0.0
 * Data	      : 02/08/2017
 * Descrição  : Projeto template para STM8S001J3
 */

/* Includes */
#include "stm8s.h"

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
  
/* not connected pins as output low state (the best EMC immunity)
(PA2, PB0, PB1, PB2, PB3, PB6, PB7, PC1, PC2, PC7, PD0, PD2, PD4, PD7, PE5, PF4) */
#define CONFIG_UNUSED_PINS_STM8S001                                                          \
{                                                                                            \
  GPIOA->DDR |= GPIO_PIN_2;                                                                  \
  GPIOB->DDR |= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7; \
  GPIOC->DDR |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;                                        \
  GPIOD->DDR |= GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_7;                           \
  GPIOE->DDR |= GPIO_PIN_5;                                                                  \
  GPIOF->DDR |= GPIO_PIN_4;                                                                  \
}

/* Declaração de Subrotinas */
static void TIM1_Config(void);
void delay(unsigned int milissegundos);

/* Variáveis globais */
volatile unsigned int u16_cnt_ms = 0;
volatile unsigned char u8_UserSW = 0;

main()
{
  /* -------------STM8S001 startup-------------- */
  /* configure unbonded pins */
  CONFIG_UNUSED_PINS_STM8S001;
  /* delay for SWIM connection: ~5seconds */
//  STARTUP_SWIM_DELAY_5S;
  /* ------------------------------------------- */
	
  //Configuração de Clock
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);	//Clock interno = 16MHz
  
  /* Inicializações */
  /* -- Pin 1 --------------------------------------------------------------- */
//  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST); //PD6/AIN6/UART1_RX
//  GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT); //PA1
  /* -- Pin 5 --------------------------------------------------------------- */
//  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST); //PB5/I2C_SDA/[TIM1_BKIN]
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);   //PA3/TIM2_CH3/[SPI_NSS]/[UART1_TX] <<<<==== LED
  /* -- Pin 6 --------------------------------------------------------------- */
//  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST); //PB4/I2C_SCL/[ADC_ETR]
  /* -- Pin 7 --------------------------------------------------------------- */
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_FL_IT);      //PC5/SPI_SCK/[TIM2_CH1] <<<<==== USER_SW
//  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); //PC4/CLK_CCO/TIM1_CH4/[AIN2]/[TIM1_CH2N]
//  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //PC3/TIM1_CH3/[TLI]/[TIM1_CH1N]
  /* -- Pin 8 --------------------------------------------------------------- */
//  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //PD5/AIN5/UART1_TX
//  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //PD3/AIN4/TIM2_CH2/ADC_ETR
//  GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT); //PD1/SWIM
//  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT); //PC6/SPI_MOSI/[TIM1_CH1]
  /* ------------------------------------------------------------------------ */
  
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_RISE_ONLY);  //Interrução externa PortaC
  TIM1_Config();
  
  //Permitir interrupções
  enableInterrupts();
  
  while(1)
  {
    switch(u8_UserSW)
    {
      case 0 :
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        delay(100);
        GPIO_WriteLow(GPIOA, GPIO_PIN_3);
        delay(100);
        break;
      
      case 1 :
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        delay(250);
        GPIO_WriteLow(GPIOA, GPIO_PIN_3);
        delay(250);
        break;
      
      case 2 :
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        delay(500);
        GPIO_WriteLow(GPIOA, GPIO_PIN_3);
        delay(500);
        break;
      
      case 3 :
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        delay(1000);
        GPIO_WriteLow(GPIOA, GPIO_PIN_3);
        delay(1000);
        break;
      
      default :
        break;
    }
  }
}

/* Subrotina  : TIM1_Config
 * Descrição  : Inicializa o timer TIM1.
 * Parâmetros : void
 * Retorno    : void
 */
static void TIM1_Config(void)
{
  //Configuração para frequência TIM1 = 1000 Hz
  uint16_t Prescaler = 15;
  uint16_t Period = 999;
  
  //Inicialização do TIM1
  TIM1_TimeBaseInit(Prescaler, TIM1_COUNTERMODE_UP, Period, 0);
  
  //Escolhe interrupção por OVERFLOW ("UPDATE")
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  
  //Ativa o periférico TIM1
  TIM1_Cmd(ENABLE);
  
  //Gera um evento para ativar as configurações
  TIM1->EGR |= 0x0001;
  
  //Limpa o flag do TIM1
  TIM1_ClearFlag(TIM1_FLAG_UPDATE);
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