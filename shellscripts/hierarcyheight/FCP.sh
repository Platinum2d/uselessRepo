#!/bin/sh
#script che fornisce il cammino massimo all'interno di una gerarchia di cartelle
#es.: ./height [percorso di una cartella]

#controllo parametri
case $# in 
   1) if test ! -d $1 -o ! -x $1 #controllo che la cartella esista e sia accessibile
      then
         echo "error: not a directory or not accessible"
         exit 1
      fi;;
   *) echo "usage: $0 [directory]" 
      exit 2;;
esac

case $1 in
   */) startpath=$1 ;;
   *) startpath=$1""/;;
esac

mem=`pwd` #memorizzo il path corrente per tornarci una volta terminata la ricorsione
PATH=`pwd`:$PATH #append del path corrente per permettere la ricorsione
export PATH

echo "0" > repval #creo il file in cui andrò a memorizzare il numero massimo temporaneo di livelli gerarchici di directory attraversati
FCR.sh 0 `pwd` $startpath #parametri: livello attuale di attraversamento
echo "Numero di livelli: `cat repval`"
rm repval #elimino il file di supporto

exit 0
