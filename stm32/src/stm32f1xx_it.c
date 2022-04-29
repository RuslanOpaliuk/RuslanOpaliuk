#include "main.h"
#include "stm32f1xx_it.h"

extern UART_HandleTypeDef uart1;


void NMI_Handler(void)
{
    while (1)
    {
        
    }
}

void stupid_delay(size_t delay)
{
    volatile size_t tmp;
    for(size_t i = 0; i < delay; ++i)
    {
        tmp++;
    }
}

void HardFault_Handler(void)
{
    __disable_irq();
    GPIO_InitTypeDef gpio_init_struct = {0};
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, 0);
    gpio_init_struct.Pin = GPIO_PIN_14;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
    
    while(1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, 1);
        stupid_delay(600000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, 0);
        stupid_delay(600000);
    }
}

void MemManage_Handler(void)
{
    while (1)
    {

    }
}

void BusFault_Handler(void)
{
    while (1)
    {

    }
}

void UsageFault_Handler(void)
{
    while (1)
    {

    }
}

void SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{

}

void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&uart1);
}
