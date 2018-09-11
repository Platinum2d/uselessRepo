#!/bin/sh

#controllo che il numero dei parametri sia pari a 1/2
if test "$#" -ne 3 
then
   echo " Utilizzo : $0 gerarchia direttorio intero"
   exit 1
fi

#controllo che il primo parametro sia espresso in forma assoluta
case "$1" in
   /*) if test ! -d "$1"
       then
          echo " Errore : $1 non è una directory"
	  exit 2
       else
	  if test ! -x "$1"
	  then
	      echo " Errore : $1 non è accessibile"
	      exit 3
          fi
       fi 
	   ;;
   *) echo " Errore : $1 non è un nome assoluto di directory"
       exit 4;;
esac	

#controllo che il secondo parametro sia un nome relativo semplice di file e che sia leggibile
case $2 in
   */*) echo " Errore : $2 non è un nome relativo semplice"
        exit 5;;
esac

#controllo che il terzo parametro sia un numero
expr $3 + 0 > /dev/null 2>&1
case $? in
   0 | 1) #controllo che il numero sia strettamente positivo 
	  if test $3 -le 0
	  then
	     echo " Errore : $4 non è strettamente positivo"
	     exit 6
	  fi;;
   *) echo " Errore : $3 non è un numero"
      exit 7;;
esac

#creo il file temporaneo
tempfile="/tmp/t$$"
touch "$tempfile"

#aggiorno la variabile d'ambiente PATH per facilitare il processo ricorsivo
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsivo
echo ""
echo "Direttori trovati:"
FCR.sh "$1" "$2" "$3" "$tempfile"

#elaboro il file temporaneo
echo ""
echo `wc -l < "$tempfile"`" file trovati"

echo ""
#elimino il file temporaneo
rm "$tempfile"

exit 0
