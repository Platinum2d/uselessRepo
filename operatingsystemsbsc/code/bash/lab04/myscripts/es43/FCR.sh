#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri
filename="$2"
tempfile="$3"

#controllo la presenza di due file di nome filename.1 e filename.2
if test -f "$filename".1 -a -r "$filename".1 -a -f "$filename".2 -a -f "$filename".2
then
  #controllo che la dimensione in linee dei due file sia identica
  
  size_1="`wc -l < "$filename.1"`"
  size_2="`wc -l < "$filename.2"`"

  if test "$size_1" -eq "$size_2"
  then
     echo `pwd` >> "$tempfile"
  fi
fi

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" 
   then
      "$0" "$i" "$2" "$3"
   fi
   
done

exit 0
