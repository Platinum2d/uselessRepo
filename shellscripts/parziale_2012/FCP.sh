#!/bin/sh

#controllo che il numero dei parametri sia pari a 1/2
if test "$#" -ne 1 
then
   echo " Utilizzo : $0 gerarchia"
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

#creo il file temporaneo
tempfile="/tmp/t$$"
echo 0 > "$tempfile"

#aggiorno la variabile d'ambiente PATH per facilitare il processo ricorsivo
PATH=`pwd`:$PATH
export PATH

#utilizzo un parametro di modalità per differenziare il tipo di chiamata ricorsiva
mode=0

#chiamata al file ricorsivo
#utilizzo un quinto parametro aggiuntivo: verrà utilizzato nella fase B
FCR.sh "$1" "0" "$tempfile" "$mode" "0"

#elaboro il file temporaneo
prof=`cat "$tempfile"`
echo ""
#chiedo all'utente un numero con le caratteristiche richieste dalla consegna
echo " Profondità: "$prof
echo ""
printf " Inserire un numero dispari compreso tra 1 e $prof : "
read n

#eseguo gli opportuni controlli sul numero inserito
expr "$n" + 0 > /dev/null 2>&1
case $? in
   0 | 1)  #controllo che il numero sia dispari
	   if test `expr "$n" % 2` -eq 0
	   then
	      echo " Errore : $n non è dispari"
	      exit 5
	   fi

	   if test "$n" -lt 1 -o "$n" -gt "$prof"
	   then 
              echo " Errore : $n non è compreso tra 1 e $prof"
	      exit 6
	   fi;;
   *) echo " Errore : $n non è un numero"
      exit 7;;
esac

#eseguo la chiamata ricorsiva in modalità 1, passando come quinto parametro (vedi sopra) il numero acquisito
mode=1
FCR.sh "$1" "0" "$tempfile" "$mode" "$n"

#elimino il file temporaneo
rm "$tempfile"

exit 0
