#include "main.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#define ADXL345_I2C_ADDR 0xA6U

#define FRAME_SIZE  12U
#define START_BYTE  0xAAU
#define STOP_BYTE   0xBBU
#define ESCAPE_BYTE 0xCCU

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;
void SystemClock_Config_16MHz(void);
void SystemClock_Config_64MHz(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_GPIO_Init(void);


static bool configure()
{
    return    HAL_I2C_IsDeviceReady(&hi2c1, ADXL345_I2C_ADDR, 1, HAL_MAX_DELAY) == HAL_OK
           && HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, 0x2C, 1, &(uint8_t){0x0F}, 1, HAL_MAX_DELAY) == HAL_OK
           && HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, 0x2D, 1, &(uint8_t){0x08}, 1, HAL_MAX_DELAY) == HAL_OK
           && HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, 0x31, 1, &(uint8_t){0x0B}, 1, HAL_MAX_DELAY) == HAL_OK;
}

//don't forget to free data out if func returned true
static bool _serialize_data(uint8_t* data_in, size_t size_in, uint8_t** data_out, size_t* size_out)
{
    if(size_in != FRAME_SIZE || !data_in || !data_out)
    {
        return false;
    }
    *size_out = size_in + 2; //+2 is for start byte at the beginning and stop byte at the end of frame

    for(size_t i = 0; i < size_in; i++)
    {
        if(   data_in[i] == START_BYTE
           || data_in[i] == STOP_BYTE
           || data_in[i] == ESCAPE_BYTE)
        {
            (*size_out)++;
        }
    }

    *data_out = (uint8_t*)malloc(*size_out);
    if(!(*data_out))
    {
        return false;
    }

    uint8_t* tmp_out = *data_out;

    tmp_out[0] = START_BYTE;
    tmp_out++;

    for(size_t i = 0; i < size_in; i++)
    {
        if(   data_in[i] == START_BYTE
           || data_in[i] == STOP_BYTE
           || data_in[i] == ESCAPE_BYTE)
        {
            *tmp_out = ESCAPE_BYTE;
            tmp_out++;
            *tmp_out = data_in[i];
            tmp_out++;
        }
        else
        {
            *tmp_out = data_in[i];
            tmp_out++;
        }
    }
    *tmp_out = STOP_BYTE;
    return true;
}

static void adxl345_routine()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
    if(!configure())
    {
        printf("can't configure sensor 1\n");
        *((int*)0) = 0;
    }

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
    if(!configure())
    {
        printf("can't configure sensor 2\n");
        *((int*)0) = 0;
    }

    uint8_t data[12];
    // uint32_t* iter = (uint32_t*)(data + 12);

    while(1)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
        if(HAL_I2C_Mem_Read(&hi2c1, ADXL345_I2C_ADDR, 0x32, 1, data, 6, HAL_MAX_DELAY) != HAL_OK)
        {
            break;
        }
        
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
        if(HAL_I2C_Mem_Read(&hi2c1, ADXL345_I2C_ADDR, 0x32, 1, data + 6, 6, HAL_MAX_DELAY) != HAL_OK)
        {
            break;
        }

        uint8_t* new_data;
        size_t new_size;
        _serialize_data(data, FRAME_SIZE, &new_data, &new_size);

        if(HAL_UART_Transmit(&huart1, new_data, new_size, HAL_MAX_DELAY) != HAL_OK)
        {
            break;
        }
        free(new_data);
        // (*iter)++;
    }
    *((int*)0) = 0;
}

int main(void)
{
    HAL_Init();
    // SystemClock_Config_16MHz();
    SystemClock_Config_64MHz();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_I2C1_Init();

    for(size_t i = 0; i < 2; i++)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
        HAL_Delay(50);
    }

    for(size_t i = 0; i < 0xFF; i++)
    {
        if(HAL_I2C_IsDeviceReady(&hi2c1, i, 2, 1000) == HAL_OK)
        {
            printf("%X\n", i);
        }
    }
    
    adxl345_routine();
    while(1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
        HAL_Delay(1000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
        HAL_Delay(1000);
    }
    
}

int _write(int fd, char* ptr, int len)
{
    for(size_t i = 0; i < len; i++)
    {
        if(*(ptr + i) == '\n')
        {
            HAL_UART_Transmit(&huart1, (uint8_t[]){ '\r', '\n' }, 2, HAL_MAX_DELAY);
            continue;
        }
        HAL_UART_Transmit(&huart1, (uint8_t*) ptr + i, 1, HAL_MAX_DELAY);
    }
    return len;
}

void SystemClock_Config_16MHz(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        *((int*)0) = 0;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK 
                                | RCC_CLOCKTYPE_SYSCLK 
                                | RCC_CLOCKTYPE_PCLK1 
                                | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        *((int*)0) = 0;
    }
}

void SystemClock_Config_64MHz(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

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
}

static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 650000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if(HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        *((int*)0) = 0;
    }
}

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 2000000;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(&huart1) != HAL_OK)
    {
        *((int*)0) = 0;
    }
}

static void _init_gpio(GPIO_TypeDef* port, uint16_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    _init_gpio(GPIOA, GPIO_PIN_0);
    _init_gpio(GPIOA, GPIO_PIN_1);
    _init_gpio(GPIOA, GPIO_PIN_2);
    _init_gpio(GPIOA, GPIO_PIN_3);
    _init_gpio(GPIOA, GPIO_PIN_4);
    _init_gpio(GPIOA, GPIO_PIN_5);
    _init_gpio(GPIOC, GPIO_PIN_15);

    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_0, 0);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_1, 0);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_2, 0);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_3, 0);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_4, 0);
    HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_5, 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
