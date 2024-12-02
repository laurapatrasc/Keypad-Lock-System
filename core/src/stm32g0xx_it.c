#include "main.h"
#include "stm32g0xx_it.h"
#include "FreeRTOS.h"
#include "task.h"

// Funcția handler pentru întreruperea Non-Maskable Interrupt (NMI)
void NMI_Handler(void)
{
   while (1) // Intră într-o buclă infinită în caz de NMI (eroare critică)
   {
   }
}

// Funcția handler pentru întreruperea HardFault
void HardFault_Handler(void)
{
  while (1) // Intră într-o buclă infinită în caz de HardFault (eroare gravă în execuție)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    // Cod personalizat pentru tratarea unei erori HardFault poate fi adăugat aici
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

// Funcția handler pentru întreruperea SysTick
void SysTick_Handler(void)
{
  HAL_IncTick(); // Incrementarea ceasului intern al HAL pentru funcționarea temporizatorilor

#if (INCLUDE_xTaskGetSchedulerState == 1) // Verifică dacă funcția de gestionare a stării schedulerului FreeRTOS este inclusă
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) // Verifică dacă schedulerul FreeRTOS este pornit
  {
#endif
    xPortSysTickHandler(); // Apel handler SysTick al FreeRTOS
#if (INCLUDE_xTaskGetSchedulerState == 1)
  }
#endif
}