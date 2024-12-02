#include "main.h"

// Funcția HAL_MspInit este apelată la inițializarea HAL pentru configurarea resurselor hardware de bază
void HAL_MspInit(void)
{
  // Activează ceasurile pentru SYSCFG și PWR
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  // Setează prioritatea pentru întreruperea PendSV utilizată de FreeRTOS
  HAL_NVIC_SetPriority(PendSV_IRQn, 3, 0);

  // Configurare pentru strobe-ul pinilor UCPD
  HAL_SYSCFG_StrobeDBattpinsConfig(SYSCFG_CFGR1_UCPD1_STROBE | SYSCFG_CFGR1_UCPD2_STROBE);
}

// Funcția pentru inițializarea hardware a perifericului UART
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0}; // Structura pentru configurarea GPIO
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0}; // Structura pentru configurarea ceasului periferic

  if(huart->Instance == USART2) // Verifică dacă se inițializează USART2
  {
    // Selectează ceasul perifericului USART2
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;

    // Configurează ceasul perifericului
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler(); // Apel funcție de eroare dacă configurarea eșuează
    }

    // Activează ceasul pentru USART2
    __HAL_RCC_USART2_CLK_ENABLE();

    // Activează ceasul pentru GPIOA (pinii utilizați de USART2 sunt pe portul A)
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configurare GPIO pentru TX (PA2) și RX (PA3)
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;  // Pini utilizați pentru TX și RX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;         // Mod alternativ funcțional (AF)
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // Fără rezistențe interne
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // Viteză joasă
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;    // Funcție alternativă: USART2
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);         // Apel HAL pentru inițializare
  }
}

// Funcția pentru dezactivarea hardware-ului perifericului UART
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance == USART2) // Verifică dacă se dezactivează USART2
  {
    // Dezactivează ceasul pentru USART2
    __HAL_RCC_USART2_CLK_DISABLE();

    // Dezactivează configurarea GPIO pentru TX (PA2) și RX (PA3)
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
  }
}