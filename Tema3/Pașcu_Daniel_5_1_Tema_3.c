/*Se consideră un program C ce conține 3 procese (procesul părinte + 2 procese fiu).
    - Timp de 5 secunde, procesul părinte citește din fișierul 'data.txt' informații pe care le transmite printr-un pipe primului proces fiu.
    - Primul proces fiu primește datele trimise de procesul părinte prin pipe și trimite toate literele mari printr-un alt pipe către 
      cel de-al doilea proces fiu. În plus, la fiecare secundă va trimite semnalul SIGUSR1 către cel de-al doilea proces fiu.
    - Cel de-al doilea proces fiu creează un fișier 'statistica.txt' în care scrie numărul total de caractere mari precum și distribuția 
      caracterelor mari (caracterul și numărul de apariții) după fiecare semnal SIGUSR1.
    - La final, procesul(al doilea proces fiu) va trimite numărul de caractere distincte printr-un alt pipe procesului părinte. 
      Procesul părinte afișează în terminal rezultatul primit de la cel de-al doilea proces fiu
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int freq[26] = { 0 };
char frq[26];
char ct[26];
char dist[26] = "";
int f2;
int k = 0;
int d = 0;

void handler(int sig)
{
	printf("ALARM!\n");
	// alarm(5);
	kill((int)getpid(),SIGINT);
}

void handler2(int sig);
void killer_handler(int sig);

int main()
{
	int pfd[2];
    int pfd2[2];
    int pfd3[2];
	int nr, nr2; 
	char ch, ch2; 
    char ch3[1024] = {'\0'};
    int i = 0;
    pid_t p1, p2;
    int f;
    char ds[2];
    char dst;
    char bufArray[4096];
    int cititi = 0;
    size_t size = sizeof(bufArray);
	
	pipe(pfd);
	pipe(pfd2);
    pipe(pfd3);
    
    if((p1 = fork())) {
        if((p2 = fork())) // Proces părinte
        {
            close(pfd[0]);
            close(pfd2[0]);

            // Dacă execuția programului sau a componenterlor lui durează 5 secunde sau mai mult, acesta se oprește
            signal(SIGALRM,handler);
	        alarm(5);
            // De exemplu, dacă sleep este decomentat, programul se oprește pentru că nu mai apucă să execute nimic
            // sleep(5);

            // Citire din fișier
            f = open("data.txt", O_RDONLY);
            if (f == -1) {
                perror("Eroare la deschiderea fișierului");
                exit(-1);
            }
            // Scriere caractere din fișier în pipe-ul pfd
            cititi = read(f, &bufArray, size);
            while(cititi)
            {
                write(pfd[1], &bufArray, cititi);
                cititi = read(f, &bufArray, size);
            }

            close(pfd[1]);
            close(pfd2[1]);
            close(f);

            close(pfd3[1]);
            
            // Citește numărul de litere mari distincte din pipe-ul pfd3 direct în d, care este 0 aici
            read(pfd3[0], &d, sizeof(int));
            printf("\nNumărul de litere mari distincte: %d\n", d);
            // Aici se citește numărul sub format de caracter într-un string
            // read(pfd3[0], &dst, 1);
            // printf("\nNumărul de litere mari distincte: %s\n", &dst);
            close(pfd3[0]);
            wait(NULL);
        }
        else // Proces fiu 2
        {
            close(pfd[1]);
            close(pfd2[1]);

            // Creare și / sau scriere în fișier
            f2 = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
            if (f2 == -1) {
                perror("Eroare la deschiderea fișierului");
                close(f);
                exit(-1);
            }

            // Citire din pipe-ul pfd2 în string-ul ch2
            nr2 = read(pfd2[0], &ch2, 1);
            while(nr2)
            {
                // Punerea fiecărei litere mari din ch2 în string-ul intermediar ch3
                strcat(&ch3[i], &ch2);
                i++;
                nr2 = read(pfd2[0], &ch2, 1);
            }

            int j = 0;
            // Parcurgerea string-ului ch3 
            while (ch3[j] != '\0') {
                freq[ch3[j] - 'A']++;  // freq[26] este folosit să stocheze numărul de apariții a fiecărei litere mari
                j++;
            }
 
            char cns[26];

            printf("Numărul total de litere mari: %d\n", i);
            
            // Scrierea numărului total de litere mari în fișier
            sprintf(cns, "%d", i);  // Transformă int în char
            write(f2, "Numărul total de litere mari: ", strlen("Numărul total de litere mari: "));
            write(f2, cns, strlen(cns));
            write(f2, "\n", strlen("\n"));
            
            // Partea asta comentată este dacă SIGUSR1 sau handler2 nu este folosit 
            /* for (k = 0; k < 26; k++) 
            {
                if (freq[k] != 0)   // Afișează numai literele mari care apar cel puțin o dată
                {
                    printf("%c - %d\n", k + 'A', freq[k]);

                    sprintf(ct, "%c", k + 'A');
                    sprintf(frq, "%d", freq[k]);

                    strcat(dist, ct);

                    // Scrie distribuția caracterelor în fișier
                    write(f2, ct, strlen(ct));
                    write(f2, " - ", strlen(" - "));
                    write(f2, frq, strlen(frq));
                    write(f2, "\n", strlen("\n"));
                }
            } */

            (void)signal(SIGUSR1, handler2);
            
            // Scrierea sau afișarea distribuției literelor după fiecare semnal SIGUSR1
            while(1) {
                kill(getpid(), SIGUSR1);
                // Cu sleep se afișează pe rând după fiecare secundă (așa se vede mai clar că e după fiecare semnal),
                // dar programul se blochează după ultima literă, fără sleep, acesta continuă mai departe
                // sleep(1); 
                if(k >= 26) break;
            }

            // Numărul de litere mari distincte; 
            d = strlen(dist); // Dacă sunt afișate și literele mari care nu apar în "data.txt", linia asta trebuie scoasă sau comentată
                                // și linia if (freq[k] != 0) d++; trebuie pusă sau decomentată în handler2
            close(pfd[0]);
            close(pfd3[0]);

            // Scrie numărul de litere mari distincte în pipe-ul pfd3 direct ca int
            write(pfd3[1], &d, sizeof(d));
            // Aici transformă numărul în caracter
            // sprintf(ds, "%d", d);
            // write(pfd3[1], &ds, strlen(ds));

            close(pfd3[1]);
            close(f2);
        }
    } 
    else // Proces fiu 1
    {
        close(pfd[1]);
        close(pfd2[0]);
        int status;

        // Citirea caracterelor din pipe-ul pfd în string-ul ch
        nr = read(pfd[0], &ch, 1);
        while(nr)
        {
            // Scrierea fiecărei litere mari din string-ul ch în pipe-ul pfd2
            if (ch >= 'A' && ch <= 'Z') {
                write(pfd2[1], &ch, 1);
            }
            nr = read(pfd[0], &ch, 1);
        }

        close(pfd[0]);
        close(pfd2[1]);

        // Trimiterea semnalului SIGUSR1 la fiecare secundă
        while (1) {
            sleep(1);
            kill(p2, SIGUSR1);
        }
		wait(&status);	

    }
    return 0;
}

void handler2(int sig)
{
    if (freq[k] != 0) {  // Afișează numai literele mari care apar cel puțin o dată
        printf("BINGO!\n");
        printf("%c - %d\n", k + 'A', freq[k]);

        sprintf(ct, "%c", k + 'A');
        sprintf(frq, "%d", freq[k]);

        strcat(dist, ct);

        write(f2, ct, strlen(ct));
        write(f2, " - ", strlen(" - "));
        write(f2, frq, strlen(frq));
        write(f2, "\n", strlen("\n"));
    }
    // if (freq[k] != 0) d++; // Linia asta trebuie decomentată dacă sunt afișate și literele care nu apar
    k++;
    (void)signal(SIGUSR1, handler2);
}
