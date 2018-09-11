
#!/bin/sh

#controllo che il numero dei parametri sia pari a 2
if test "$#" -ne 3 
then
   echo " Utilizzo : $0 gerarchia carattere carattere"
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

#controllo che gli altri due parametri siano solo dei caratteri
case "$2" in
	?) ;;
	*) echo " Errore : $2 non è un singolo carattere"
           exit 5;;
esac

case "$3" in
	?);;
	*) echo " Errore : $3 non è un singolo carattere" 
	   exit 6;;
esac

#creo il file temporaneo
tempfile="/tmp/t$$"
touch "$tempfile"

#aggiorno la variabile d'ambiente PATH per facilitare il processo ricorsivo
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsivo
FCR.sh "$1" "$2" "$3" "$tempfile"

#elaboro il file temporaneo
echo ""
echo `wc -l < "$tempfile"`" directories trovate"
cat "$tempfile"

#elimino il file temporaneo
rm "$tempfile"

exit 0

