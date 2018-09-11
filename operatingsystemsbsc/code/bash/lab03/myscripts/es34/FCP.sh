
#!/bin/sh

#controllo che il numero dei parametri sia pari a 1/2
if test "$#" -ne 2
then
   echo " Utilizzo : $0 gerarchia intero_positivo"
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

#controllo che il secondo parametro sia un intero positivo
expr "$2 + 0" > /dev/null 2>&1
case "$?" in
   0 | 1) if test $2 -le 0 
          then
             echo " Errore : $2 non è positivo"
	     exit 5
          fi;;
   *) echo " Errore : $2 non è un numero"
      exit 6;;
esac

#creo il file temporaneo
tempfile="/tmp/t$$"
touch "$tempfile"

#aggiorno la variabile d'ambiente PATH per facilitare il processo ricorsivo
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsivo
FCR.sh "$1" "$2" "$tempfile"

#elaboro il file temporaneo


#elimino il file temporaneo
rm $tempfile

exit 0
