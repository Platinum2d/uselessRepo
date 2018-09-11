#!/bin/sh

#mi sposto nel direttorio da analizzare
cd "$1"

#acquisisco i parametri

D="$2"
N="$3"
tmp="$4"
printed=0

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      "$0" "$i" "$2" "$3" "$4"
   fi

   #controllo che il direttorio attuale si chiami D e che l'elemento corrente sia un file
   if test "$1" = "$D" -a -f "$i"
   then
      #controllo che la dimensione del file sia superiore a N
      if test `wc -c < "$i"` -gt $N
      then
	      #il file è idoneo: stampo il percorso assoluto del direttorio (se non l'ho già fatto) e memorizzo il file ove necessario
              if test $printed -eq 0
	      then
	          printed=1
		  echo `pwd`
	      fi
	      echo `cat "$i"` >> SOMMA
              echo `pwd`/"$i" >> "$tmp"
      fi
   fi
done

exit 0
