#include "stm32g0xx.h"

// Definire valori implicite pentru sursele de ceas dacă nu sunt deja definite
#if !defined  (HSE_VALUE)
#define HSE_VALUE    (8000000UL)    // Frecvența ceasului extern HSE (High-Speed External) 8 MHz
#endif 

#if !defined  (HSI_VALUE)
  #define HSI_VALUE  (16000000UL)   // Frecvența ceasului intern HSI (High-Speed Internal) 16 MHz
#endif 

#if !defined  (LSI_VALUE)
 #define LSI_VALUE   (32000UL)      // Frecvența ceasului intern LSI (Low-Speed Internal) 32 kHz
#endif 

#if !defined  (LSE_VALUE)
  #define LSE_VALUE  (32768UL)      // Frecvența ceasului extern LSE (Low-Speed External) 32.768 kHz
#endif 


// Configurare opțională a adresei bazei tabelului vectorilor de întrerupere
#if defined(USER_VECT_TAB_ADDRESS)

#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   SRAM_BASE        // Tabelul vectorilor se află în SRAM
#define VECT_TAB_OFFSET         0x00000000U     // Fără decalare
#else
#define VECT_TAB_BASE_ADDRESS   FLASH_BASE       // Tabelul vectorilor se află în FLASH
#define VECT_TAB_OFFSET         0x00000000U     // Fără decalare
#endif 

#endif 

// Variabila globală pentru frecvența curentă a nucleului
uint32_t SystemCoreClock = 16000000UL;

// Tabele de prescalere pentru AHB și APB (frecvențe reduse prin divizoare)
const uint32_t AHBPrescTable[16UL] = {0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 1UL, 2UL, 3UL, 4UL, 6UL, 7UL, 8UL, 9UL};
const uint32_t APBPrescTable[8UL] =  {0UL, 0UL, 0UL, 0UL, 1UL, 2UL, 3UL, 4UL};

// Funcția pentru inițializarea sistemului
void SystemInit(void)
{
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; // Configurează adresa bazei tabelului vectorilor
#endif 
}

// Funcția pentru actualizarea variabilei `SystemCoreClock` cu frecvența curentă
void SystemCoreClockUpdate(void)
{
  uint32_t tmp;         // Variabilă temporară pentru prescaler
  uint32_t pllvco;      // Frecvența de ieșire a PLL-ului
  uint32_t pllr;        // Divizorul PLL pentru ieșire
  uint32_t pllsource;   // Sursa PLL-ului (HSI sau HSE)
  uint32_t pllm;        // Divizorul PLL de intrare
  uint32_t hsidiv;      // Divizorul HSI

  // Determină sursa ceasului activ conform registrului RCC_CFGR
  switch (RCC->CFGR & RCC_CFGR_SWS)
  {
    case RCC_CFGR_SWS_0:                 // HSE este sursa de ceas
      SystemCoreClock = HSE_VALUE;
      break;

    case (RCC_CFGR_SWS_1 | RCC_CFGR_SWS_0):  // LSI este sursa de ceas
      SystemCoreClock = LSI_VALUE;
      break;

    case RCC_CFGR_SWS_2:                 // LSE este sursa de ceas
      SystemCoreClock = LSE_VALUE;
      break;

    case RCC_CFGR_SWS_1:                 // PLL este sursa de ceas
      // Determină sursa PLL
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
      pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos) + 1UL;

      if(pllsource == 0x03UL)            // Dacă sursa PLL este HSE
      {
        pllvco = (HSE_VALUE / pllm);
      }
      else                               // Dacă sursa PLL este HSI
      {
          pllvco = (HSI_VALUE / pllm);
      }

      // Calculează frecvența VCO-ului PLL
      pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);
      pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_Pos)