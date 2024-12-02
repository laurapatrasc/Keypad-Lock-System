#include <errno.h>    // Pentru gestionarea erorilor (errno)
#include <stdint.h>   // Pentru tipuri de date standard (uint8_t, uint32_t)

// Variabilă statică pentru a ține evidența sfârșitului heap-ului
static uint8_t *__sbrk_heap_end = NULL;

void *_sbrk(ptrdiff_t incr)
{
  // Simboluri definite de linker pentru limitele memoriei:
  extern uint8_t _end;             // Sfârșitul secțiunii `.bss` (începutul heap-ului)
  extern uint8_t _estack;          // Adresa vârfului stivei
  extern uint32_t _Min_Stack_Size; // Dimensiunea minimă rezervată pentru stivă
  
  // Calculul limitei superioare a heap-ului (în funcție de dimensiunea minimă a stivei)
  const uint32_t stack_limit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
  const uint8_t *max_heap = (uint8_t *)stack_limit;

  // Variabilă pentru a păstra valoarea anterioară a sfârșitului heap-ului
  uint8_t *prev_heap_end;

  // Inițializarea variabilei `__sbrk_heap_end` dacă este NULL (prima apelare)
  if (NULL == __sbrk_heap_end)
  {
    __sbrk_heap_end = &_end; // Heap-ul începe de la sfârșitul secțiunii `.bss`
  }

  // Verificare: se evită suprapunerea între heap și stivă
  if (__sbrk_heap_end + incr > max_heap)
  {
    errno = ENOMEM; // Setează eroarea "Not enough memory"
    return (void *)-1; // Returnează eroare
  }

  // Stochează valoarea curentă a sfârșitului heap-ului
  prev_heap_end = __sbrk_heap_end;

  // Ajustează sfârșitul heap-ului cu `incr` (creștere sau scădere)
  __sbrk_heap_end += incr;

  // Returnează adresa inițială a heap-ului înainte de ajustare
  return (void *)prev_heap_end;
}