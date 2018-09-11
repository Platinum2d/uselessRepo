#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri
level="$2"
tmp="$3"
mode="$4"
req="$5"

#controllo che il livello attuale di profondità di gerarchia sia maggiore di quello attualmente memorizzato nel file temporaneo
if test `cat "$tmp"` -lt "$level"
then
   echo "$level" > "$tmp"
fi

#se la directory attuale si trova nel livello richiesto e la modalità è quella della fase B, descrivo tutti i file della cartella (inclusi quelli nascosti)
if test "$level" -eq "$req" -a "$mode" -eq 1
then
   echo " ##### `pwd` #####"
   ls -ial
fi

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      level=`expr $level + 1`
      "$0" "$i" "$level" "$3" "$4" "$5"
   fi
done

exit 0
