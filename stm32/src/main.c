#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <memory.h>

UART_HandleTypeDef uart1;


static void gpio_init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init_struct = {0};
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio_init_struct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
}

static void uart1_init(void)
{
    uart1.Instance = USART1;
    uart1.Init.BaudRate = 2000000;
    uart1.Init.WordLength = UART_WORDLENGTH_8B;
    uart1.Init.StopBits = UART_STOPBITS_1;
    uart1.Init.Parity = UART_PARITY_NONE;
    uart1.Init.Mode = UART_MODE_TX_RX;
    uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(&uart1) != HAL_OK)
    {
        *((int*)0) = 0;
    }
}

void clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL7;

    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        *((int*)0) = 0;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        *((int*)0) = 0;
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;

    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        *((int*)0) = 0;
    }
}

int _write(int fd, char* ptr, int len)
{
    for(size_t i = 0; i < len; i++)
    {
        if(*(ptr + i) == '\n')
        {
            HAL_UART_Transmit(&uart1, (uint8_t[]){ '\r', '\n' }, 2, HAL_MAX_DELAY);
            continue;
        }
        HAL_UART_Transmit(&uart1, (uint8_t*) ptr + i, 1, HAL_MAX_DELAY);
    }
    return len;
}

int main(void)
{
    HAL_Init();
    clock_config();
    gpio_init();
    uart1_init();

    while(1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
        HAL_Delay(100);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
        HAL_Delay(100);
    }
}
