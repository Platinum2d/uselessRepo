#!/bin/sh

#controllo che il numero dei parametri sia pari a n.
if test $# -ne 2
then
   echo " Utilizzo : $0 gerarchia file/intero"
   exit 1
fi

#controllo che il primo parametro sia espresso in forma assoluta.
case $1 in
   /*) #controllo che il primo parametro si riferisca ad un direttorio.
       if test ! -d $1
       then
          echo " Errore : $1 non è una directory"
	  exit 2
       else
	  #controllo che il primo parametro si riferisca ad un direttorio accessibile.
	  if test ! -x $1
	  then
	      echo " Errore : $1 non è accessibile"
	      exit 3
          fi
       fi;;
   *) echo " Errore : $1 non è un nome assoluto di directory"
       exit 4;;
esac	

#controllo che il secondo parametro sia un nome relativo semplice. 
case $2 in
   */*) echo " Errore : $2 non è un nome relativo semplice"
        exit 5;;
esac

#controllo che il secondo parametro sia un numero.
expr $2 + 0
case $? in
   0 | 1) #controllo che il secondo parametro sia strettamente positivo.
	  if test $2 -le 0 
          then
             echo " Errore : $2 non è strettamente positivo "
	     exit 6
          fi;;
   *) echo " Errore : $2 non è un numero intero"
      exit 7;;
esac

#creo il file temporaneo, fruibile ad ogni iterazione ricorsiva. Il PID in coda al nome del file serve a rendere quest'ultimo esclusivo delprocesso che lo esegue, scongiurando conflitti con altre esecuzioni dello script.
tempfile=/tmp/t$$
touch $tempfile

#aggiorno la variabile d'ambiente PATH per facilitare il processo ricorsivo.
PATH=`pwd`:$PATH
export PATH

#eseguo la chimata al file ricorsivo.
FCR.sh $1 $2 $tempfile

#elaboro il file temporaneo.
echo ""
echo `wc -l < $tempfile`" robe trovate"
cat $tempfile

#elimino il file temporaneo.
rm $tempfile

#esco dallo script con valore 0 in quanto l'esecuzione non ha presentato alcun tipo di errore se giunta fino a questo punto.
exit 0
