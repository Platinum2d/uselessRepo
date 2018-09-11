#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri
dirname="$2"
tmpfile="$3"


#se è il caso, opero sui file con un ciclo esterno a quello delle directory. Altrimenti: usare quello standard
if test -d "$dirname" -a -x "$dirname"
then
   #controllo che la cartella corrente abbia come nome quello che sto cercando
   #ricavo il numero di file
   nf=`ls -l "$dirname" | grep ^[-] | wc -l`

   #ricavo il numero di directory
   nd=`ls -l "$dirname"| grep ^[d] | wc -l`

   #confronto i due valori come richiesto
   if test $nf -eq `expr $nd + 2`
   then 
      echo `pwd`/$i >> "$tmpfile"
     
      echo ""
      echo "Trovata cartella compatibile: "`pwd`/"$dirname" 
      read -p "Inserire il nome della prima directory: " firstd
      read -p "Inserire il nome della seconda directory: " secondd
      if test "$secondd" = "$firstd"
      then
         secondd="$secondd.1"
	 echo " Attenzione : il nome delle directories è identico. La seconda verrà nominata $secondd"
      fi
      mkdir `pwd`/"$dirname"/"$firstd"
      mkdir `pwd`/"$dirname"/"$secondd"
   fi
fi

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i"
   then
      "$0" "$i" "$dirname" "$tmpfile"
   fi
done

exit 0
