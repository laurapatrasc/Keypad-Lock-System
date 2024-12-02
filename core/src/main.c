#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>

// Declarație pentru UART
UART_HandleTypeDef huart2;

// Definiții pentru rândurile tastaturii matriciale
GPIO_TypeDef* rowPorts[4] = {GPIOA, GPIOA, GPIOA, GPIOA};
uint16_t rowPins[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_4, GPIO_PIN_6};

// Definiții pentru coloanele tastaturii matriciale
GPIO_TypeDef* colPorts[4] = {GPIOB, GPIOB, GPIOB, GPIOB};
uint16_t colPins[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

// Matricea tastaturii 4x4 (mapare rând-coloană)
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void SystemClock_Config(void);  // Configurarea ceasului sistemului
static void MX_GPIO_Init(void); // Inițializare GPIO
static void MX_USART2_UART_Init(void); // Inițializare UART
void KeypadTask(void *argument); // Task pentru gestionarea tastaturii matriciale

char read_keypad(void); // Funcție pentru citirea tastaturii matriciale

// Funcția care citește apăsările de taste
char read_keypad(void) {
    for (int row = 0; row < 4; row++) {
        // Activează toate rândurile (le setează HIGH)
        for (int r = 0; r < 4; r++) {
            HAL_GPIO_WritePin(rowPorts[r], rowPins[r], GPIO_PIN_SET);
        }

        // Activează rândul curent (setat LOW)
        HAL_GPIO_WritePin(rowPorts[row], rowPins[row], GPIO_PIN_RESET);

        // Verifică fiecare coloană pentru apăsări de taste
        for (int col = 0; col < 4; col++) {
            if (HAL_GPIO_ReadPin(colPorts[col], colPins[col]) == GPIO_PIN_RESET) {
                vTaskDelay(pdMS_TO_TICKS(50)); // Debounce (50 ms)
                while (HAL_GPIO_ReadPin(colPorts[col], colPins[col]) == GPIO_PIN_RESET); // Așteaptă eliberarea tastei
                return keymap[row][col]; // Returnează caracterul apăsat
            }
        }
    }
    return '\0';  // Nicio tastă apăsată
}

// Task pentru gestionarea tastaturii
void KeypadTask(void *argument) {
    char key;          // Variabilă pentru a stoca tasta apăsată
    char message[16];  // Mesaj pentru transmiterea UART

    for (;;) {
        key = read_keypad(); // Citește tasta
        if (key != '\0') {   // Dacă o tastă este detectată
            snprintf(message, sizeof(message), "Key: %c\r\n", key); // Formatează mesajul
            HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY); // Trimite mesajul prin UART
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Task delay (100 ms)
    }
}

// Funcția principală
int main(void) {
    HAL_Init(); // Inițializare HAL (Hardware Abstraction Layer)
    SystemClock_Config(); // Configurarea ceasului sistemului
    MX_GPIO_Init(); // Inițializare GPIO
    MX_USART2_UART_Init(); // Inițializare UART

    // Creează un task pentru gestionarea tastaturii
    xTaskCreate(KeypadTask,        // Funcția task-ului
                "KeypadTask",      // Nume task
                128,               // Dimensiunea stivei
                NULL,              // Parametri (NULL)
                1,                 // Prioritatea task-ului
                NULL);             // Handler-ul task-ului (NULL)

    vTaskStartScheduler(); // Pornește FreeRTOS scheduler-ul

    while (1) {
        // Bucle infinită în cazul în care scheduler-ul eșuează
    }
}

// Configurarea ceasului sistemului
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1); // Scalare tensiune pentru consum redus

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI; // Configurare ceas intern (HSI)
    RCC_OscInitStruct.HSIState = RCC_HSI_ON; // Activează HSI
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1; // Divizor HSI
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // Calibrare implicită
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // PLL dezactivat
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { // Verificare configurare
        Error_Handler(); // Eroare în cazul eșecului
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; // Tipuri de ceas
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; // Sursă SYSCLK: HSI
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // Divizor AHB
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; // Divizor APB1

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) { // Verificare configurare
        Error_Handler(); // Eroare în cazul eșecului
    }
}

// Inițializare UART
static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2; // Se utilizează USART2
    huart2.Init.BaudRate = 115200; // Baud rate 115200
    huart2.Init.WordLength = UART_WORDLENGTH_8B; // Lungime cuvânt: 8 biți
    huart2.Init.StopBits = UART_STOPBITS_1; // 1 bit de stop
    huart2.Init.Parity = UART_PARITY_NONE; // Fără paritate
    huart2.Init.Mode = UART_MODE_TX_RX; // Mod transmisie și recepție
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; // Fără control hardware al fluxului
    huart2.Init.OverSampling = UART_OVERSAMPLING_16; // Oversampling 16x
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE; // Dezactivare probă un bit
    huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1; // Prescaler divizor 1
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; // Fără caracteristici avansate
    if (HAL_UART_Init(&huart2) != HAL_OK) { // Verificare inițializare
        Error_Handler(); // Eroare în cazul eșecului
    }
}

// Inițializare GPIO
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Activează ceasurile pentru porturi GPIO
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configurare pinii pentru rânduri (GPIOA, OUTPUT_PP)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configurare pinii pentru coloane (GPIOB, INPUT_PULLUP)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// Funcția pentru tratarea erorilor
void Error_Handler(void) {
    __disable_irq(); // Dezactivează toate întreruperile
    while (1) {
        // Buclă infinită pentru indicarea erorii
    }
}