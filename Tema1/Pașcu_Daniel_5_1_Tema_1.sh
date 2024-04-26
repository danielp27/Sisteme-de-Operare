#! /bin/bash

# Să se realizeze un script ce colectează și afișează informații despre sistemul de operare. 
# Scriptul va afișa meniul și va aștepta până când utilizatorul introduce o opțiune validă. 
# După fiecare acțiune, utilizatorul va reveni în meniu pentru a putea alege o altă opțiune sau pentru a ieși din program. 
# Scriptul va afișa un meniu cu următoarele opțiuni:
#    1. Ieșire
#    2. Afișare informații despre sistemul de operare: numele serverului, tipul sistemului de operare, versiunea bash-ului 
#       și calea curentă (vezi $HOSTNAME, $OSTYPE si alte variabile de sistem)
#    3. Afișarea ultimelor 3 shell-urilor instalate în sistem (vezi /etc/shells)
#    4. Afișare informații despre rețea: afișarea adresei IP a interfeței de rețea și informații despre gateway (vezi ifconfig și route)
#    5. Creare director & Copiere fișier: să se permită utilizatorului să introducă numele unui director și să creeze acel director 
#       în directorul curent, să ai posibilitatea să introduci numele unui fișier (citit de la tastatură) și să îl copieze în 
#       directorul creat anterior.
#    6. Ștergere director: să se permită utilizatorului să introducă numele directorului create la punctul 5 și să șteargă 
#       acel director și toate fișierele din interiorul lui
#    7. Afișare memorie: afișarea memoriei totale și a memoriei libere a sistemului de operare.
#    8. Căutare comandă: să se permită utilizatorului să introducă un cuvânt cheie și să afișeze comenzile care conțin acest 
#       cuvânt cheie în numele lor – comenzi care au fost executate înainte in terminal ( vezi history)
#    9. Sa se afișeze toate fișierele de log. Sa se creeze o funcție care primește ca si parametrii citiți de la tastatura un 
#       nume de fișier de log (ori curent ori arhivat) și un text, si care va căuta si afișa liniile care conțin textul primit ca si parametru.


if [ $SHELL == "/bin/bash" ]
then
    HISTFILE=~/.bash_history
elif [ $SHELL == "/bin/zsh" ]
then
    HISTFILE=~/.zsh_history
fi
set -o history

log() {
    if [ $3 == "gz" ]
    then
        gunzip -c /var/log/"$1" | grep "$2"
    else
        cat /var/log/"$1" | grep "$2"
    fi
}

i=1
while [ $i -ne 0 ]
do
	echo -e "Press enter to continue...\c"
	read dummy

	clear
	echo "1. Ieșire"
	echo "2. Afișare informații despre sistemul de operare"
	echo "3. Afișarea ultimelor 3 shell-urilor instalate în sistem"
    echo "4. Afișare informații despre rețea"
    echo "5. Creare director & Copiere fișier"
    echo "6. Ștergere director"
    echo "7. Afișare memorie"
    echo "8. Căutare comandă"
    echo -e "9. Sa se afișeze toate fișierele de log\n"
	
    echo -e "Introduceți opțiunea: \c"
    read i

	case $i in
		1) break;;
		2) echo "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
           echo -e "\e[0;31m Numele serverului: $(hostname)"
           echo -e "\e[0;32m Tipul sistemului de operare: $(uname -o)"
           echo -e "\e[0;33m Versiune bash: $(bash --version)"
           echo -e "\e[0;34m Calea curentă: $(echo $PATH)"
           echo -e "\e[0m----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";;
		3) echo "----------"
           cat /etc/shells | grep usr | tail -n 3
           echo "----------";;
        4) echo "--------------------------------"
           echo -e "IP: $(ifconfig | grep inet | awk '{print $2}' | head -n 3 | tail -n 1)\n"
         #   echo -e "IP: $(hostname -I)\n"
           route | awk '{printf "%-15s %-15s\n", $1, $2}' | tail -n 4
           echo "--------------------------------";;
        5) j=1
           while [ $j -ne 0 ]
           do
                echo -e "Press enter to continue...\c"
                read dummy

                clear
                echo "1. Ieșire"
                echo "2. Creare director în directorul curent"
                echo -e "3. Copiere fișier în directorul creat\n"

                echo -e "Introduceți opțiunea: \c"
                read i

                case $i in
                    1) break;;
                    2) echo -e "Nume director: \c"
                       read dir
                       if [ ! -d "$dir" ]
                       then
                            mkdir "$dir"
                       fi;;
                    3) echo -e "Nume sau cale fișier: \c"
                       read f
                       cp "$f" "$dir";;
                    *) echo "Opțiune greșită!";;
                esac
           done;;
        6) echo -e "Nume director: \c"
           read dir
           rm -rf "$dir";;
        7) echo "--------------------------"
           echo "Memorie totală: $(free -m | head -n 2 | awk '{print $2}' | tail -n 1) Mb"
           echo "Memorie liberă: $(free -m | head -n 2 | awk '{print $4}' | tail -n 1) Mb"
           echo "Memorie valabilă: $(free -m | head -n 2 | awk '{print $7}' | tail -n 1) Mb"
           echo "--------------------------";;
        8) echo -e "Cuvânt cheie = \c"
           read cv
           history | grep $cv;;
        9) ls /var/log
           echo -e "\nNume fișier log: \c"
           read fl
           echo -e "Text căutat: \c"
           read tx
           echo -e "\nLiniile care conțin textul căutat: \n"
           fis=$(basename -- "$fl")
           ext="${fis##*.}"
           log $fl $tx $ext;;
		*) echo "Opțiune greșită!";;
	esac
done
