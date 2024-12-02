#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

// Funcții slabe definite pentru operațiile de I/O personalizate
extern int __io_putchar(int ch) __attribute__((weak));  // Funcție pentru scriere caracter
extern int __io_getchar(void) __attribute__((weak));   // Funcție pentru citire caracter

// Definirea variabilelor de mediu (necesar pentru compatibilitate POSIX)
char *__env[1] = { 0 };  // Mediu gol
char **environ = __env;  // Pointer către mediu

// Funcție de inițializare pentru debugger (folosită cu instrumente precum semihosting)
void initialise_monitor_handles()
{
  // În acest caz, nu face nimic. Este folosită pentru implementări avansate.
}

// Funcția care returnează ID-ul procesului (compatibilitate POSIX)
int _getpid(void)
{
  return 1;  // ID-ul fixat la 1, deoarece rulează pe un microcontroler
}

// Funcție pentru terminarea unui proces specific
int _kill(int pid, int sig)
{
  (void)pid;  // Ignoră parametrii
  (void)sig;
  errno = EINVAL;  // Setează eroarea "Invalid argument"
  return -1;       // Returnează eșec
}

// Funcție pentru terminarea programului
void _exit (int status)
{
  _kill(status, -1);  // Apel către _kill
  while (1) {}        // Intră într-o buclă infinită
}

// Funcție slabă pentru citirea datelor (suprascrisă dacă e nevoie)
__attribute__((weak)) int _read(int file, char *ptr, int len)
{
  (void)file;  // Ignoră descriptorul de fișier
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    *ptr++ = __io_getchar();  // Citește caracter cu __io_getchar
  }

  return len;  // Returnează numărul de caractere citite
}

// Funcție slabă pentru scrierea datelor (suprascrisă dacă e nevoie)
__attribute__((weak)) int _write(int file, char *ptr, int len)
{
  (void)file;  // Ignoră descriptorul de fișier
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    __io_putchar(*ptr++);  // Scrie caracter cu __io_putchar
  }
  return len;  // Returnează numărul de caractere scrise
}

// Închidere fișier (neimplementată pentru microcontroler)
int _close(int file)
{
  (void)file;
  return -1;  // Operația nu este suportată
}

// Statistici pentru fișier (returnează că este un dispozitiv de caracter)
int _fstat(int file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;  // Setează tipul ca dispozitiv de caracter
  return 0;  // Operația este reușită
}

// Verifică dacă fișierul este un terminal
int _isatty(int file)
{
  (void)file;
  return 1;  // Toate fișierele sunt tratate ca terminale
}

// Repozitionare fișier (neimplementată)
int _lseek(int file, int ptr, int dir)
{
  (void)file;
  (void)ptr;
  (void)dir;
  return 0;  // Operația este considerată reușită
}

// Deschidere fișier (neimplementată)
int _open(char *path, int flags, ...)
{
  (void)path;
  (void)flags;
  return -1;  // Operația nu este suportată
}

// Așteptare proces (neimplementată)
int _wait(int *status)
{
  (void)status;
  errno = ECHILD;  // Setează eroarea "No child processes"
  return -1;
}

// Ștergere fișier (neimplementată)
int _unlink(char *name)
{
  (void)name;
  errno = ENOENT;  // Setează eroarea "No such file or directory"
  return -1;
}

// Obține timpii de execuție (neimplementată)
int _times(struct tms *buf)
{
  (void)buf;
  return -1;
}

// Obține informații despre un fișier (returnează că este un dispozitiv de caracter)
int _stat(char *file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;  // Setează tipul ca dispozitiv de caracter
  return 0;
}

// Creează un link simbolic (neimplementat)
int _link(char *old, char *new)
{
  (void)old;
  (void)new;
  errno = EMLINK;  // Setează eroarea "Too many links"
  return -1;
}

// Creează un proces copil (neimplementat)
int _fork(void)
{
  errno = EAGAIN;  // Setează eroarea "Resource temporarily unavailable"
  return -1;
}

// Executează un nou proces (neimplementat)
int _execve(char *name, char **argv, char **env)
{
  (void)name;
  (void)argv;
  (void)env;
  errno = ENOMEM;  // Setează eroarea "Not enough memory"
  return -1;
}