#include "main.h"
#include "stm32f1xx_it.h"

extern UART_HandleTypeDef huart1;


void NMI_Handler(void)
{
    while (1)
    {
        
    }
}

void stupid_delay(size_t delay)
{
    volatile size_t tmp;
    for(size_t i = 0; i < delay; i++)
    {
        tmp++;
    }
}

void HardFault_Handler(void)
{
    __disable_irq();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, 0);
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
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
    HAL_UART_IRQHandler(&huart1);
}
