/* Realizați un program în limbajul C care primește 3 argumente din linia de comandă: 
un director sursă, un director destinație și un număr natural pozitiv.
Se va parcurge recursiv structura de directoare din directorul sursă. 
Pentru fiecare intrare din directorul sursă, se vor executa următoarele operații, în funcție de tipul intrării:
    Pentru directoare, se va crea un director echivalent în directorul destinație, cu aceleași drepturi ca directorul original. 
	 Astfel, structura de directoare din directorul destinație va fi asemănătoare cu cea din sursă.
    Pentru fișierele obișnuite, în funcție de extensia fișierului:
    	Pentru fișierele cu extensia .txt, programul va calcula numărul de spații pe care îl conține fișierul. 
		 Dacă numărul de spații al acelui fișier este mai mare decât numărul dat ca al doilea argument, 
		 programul va crea o legătură simbolică către acel fișier, în directorul destinație (sau din interiorul acestuia, 
		 în funcție de structura de mai sus), cu numele fișierului original, dar cu sufixul "_spaces". 
		 Aceste link-uri vor avea aceleași drepturi de acces ca și fișierele originale.
        Exemplu: dacă un fișier numit "file1.txt" conține mai multe spații decât numărul dat ca argument, 
		programul va crea o legătură simbolică către acel fișier, cu numele "file1.txt_spaces”.
        Pentru fișierele care au altă extensie, se va crea o copie a fișierului în directorul destinație 
		 (și, după caz, în subfolderul echivalent). Aceste fișiere copie vor avea aceleași 
		 drepturi de read ca și fișierele originale (Exemplu: dacă fișierul original are –rwx-wxr-x, fișierul nou va avea –r-----r--).
        Pentru (sof)link-uri, nu se va executa nicio operație.
Tema va fi însoțită de fișierul Makefile corespunzător. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int a = 0;

int copyfiles(const char *sname, const char *dname, struct stat *s)
{	
	int f, f2;
    int cititi = 0;
    char bufArray[1000];
	size_t size = sizeof(bufArray);

	f = open(sname, O_RDONLY);
	if (f == -1) {
        perror("Eroare la deschiderea fișierului sursă (copyfiles)");
        return -1;
    }

	f2 = open(dname, O_WRONLY | O_CREAT | O_TRUNC, s->st_mode);

	if (f2 == -1) {
        perror("Eroare la deschiderea fișierului sursă");
        close(f);
        return -1;
    }
	while ((cititi = read(f, &bufArray, size))) {
		bufArray[cititi] = '\0';
		write(f2, bufArray, cititi); 
	}

	if (cititi == -1) {
        perror("Eroare la citirea fișierului sursă");
        close(f);
        close(f2);
        return -1;
    }

	close(f);
	close(f2);

	//success
	return 1;
}


void parcurg(char *nume, char *dest, const int *nr)
{
    DIR *dr;
	struct dirent *entryData;
	struct stat stare;
	char path1[1000], path2[1000], path3[1000];
	char delimiter[] = ".";
    char *token;
    char *format;
	int f;
    int cititi = 0;
    char bufArray[1000];
	int count;
	size_t size = sizeof(bufArray);

	dr = opendir (nume);
	if(dr == NULL)
	{	
		perror ("Directorul trebuie introdus in linia de comanda \n");
		exit(0);
	}
	
	stat(nume,&stare);
	// stat(dest,&stare2);
	if(!S_ISDIR(stare.st_mode)) {
		printf("%s nu este director, introduceți un director\n", nume);
		exit(0); 
	} 

	mkdir(dest, stare.st_mode);

    while((entryData = readdir (dr)) != NULL)
	{
		// Excludere directorii curente sau directorii părinte
		if (strcmp(entryData->d_name, ".") == 0 || strcmp(entryData->d_name, "..") == 0)
            continue;

		// Construire căi sursă
		strcpy(path1,"");
		stpcpy(path1,nume);
		strcat(path1,"/");
		strcat(path1,entryData->d_name);

		printf("\n__________________________________________________");
		printf("\npath1 = %s", path1);

		// Construire căi destinație
		strcpy(path2,"");
		stpcpy(path2,dest);
		strcat(path2, "/");
		
		if (a == 0) {
			// Pentru fișiere
			strcat(path2, nume);
			mkdir(path2, stare.st_mode); // Creare director cu același nume ca directorul sursă în directorul destinație
			strcat(path2, "/");
			strcat(path2, entryData->d_name);
			printf("\npath2_0 = %s", path2);
		} else {
			// Pentru fișiere din subdirectorii
			strcat(path2, entryData->d_name);
			printf("\npath2_1 = %s", path2);
		}
		
		printf("\na = %d", a);
		
		lstat(path1, &stare);
		if(S_ISDIR(stare.st_mode)) {
			a = 1;
			parcurg(path1, path2, nr); 
			a = 0; 
		} else if(S_ISREG(stare.st_mode)) {
			// Extragere format fișier
			token = strtok(path1, delimiter);
			while (token != NULL) {
        		format = token;
        		token = strtok(NULL, delimiter);
    		}
			strcpy(path3, format);

			if (strcmp(format, "txt") == 0) {
				f = open(strcat(path1, ".txt"), O_RDONLY); // path1 a rămas fără extensia .txt
				if (f == -1) {
					perror("Eroare la deschiderea fișierului sursă");
					exit(-1);
				}
				size_t size = sizeof(bufArray);
				count = 0;

				printf("\nConținutul fișierului:\n");
				printf("---------------------------\n");
				while((cititi = read(f, &bufArray, size)))
				{
					bufArray[cititi]='\0';
					printf("%s", bufArray);

					// Se numără de câte ori apare spațiu în fișier
					for (int i = 0; i < cititi; i++) {
						if (bufArray[i] == ' ') {
							count ++;
						}
					}    
				}
				printf("\n---------------------------");
				printf("\nformat = %s", format);
				if (cititi == -1) {
					perror("Eroare la citirea fișierului sursă");
					close(f);
					exit(0);
				}
				close(f);
				printf("\nNumărul de spații este %d", count);
				if (count > *nr) {
					symlink(path1, strcat(path2, "_spaces"));
					stat(path2, &stare);
					chmod(path2, stare.st_mode);
					printf("\nsymlink = %s", path2);
					printf("\n__________________________________________________\n");
				}
			} else if (strcmp(path1, path3) == 0) { // Fișierele fără terminație vor avea același nume cu calea lor
				stat(path2, &stare);
				copyfiles(path1, path2, &stare);
				chmod(path2, stare.st_mode);
				printf("\nformat = %s", path3);
				printf("\n__________________________________________________\n");
			} else {
				stat(path2, &stare);
				strcat(path1, ".");
				strcat(path1, path3);
				printf("\nformat = %s", path3);
				copyfiles(path1, path2, &stare);
				chmod(path2, stare.st_mode);
				printf("\n__________________________________________________\n");
			}
		} else if (S_ISLNK(stare.st_mode)) {
			continue;
		}
	}
	closedir(dr);
}

void main(int argc, char * argv[])
{
	int x;
	sscanf(argv[3], "%d", &x);
	parcurg(argv[1], argv[2], &x);	
}
