
#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri

#se è il caso, opero sui file con un ciclo esterno a quello delle directory. Altrimenti: usare quello standard
D="$2"
tmp=$3
mode=$4

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      if test "$i" = "$D" -a $mode -eq 0
      then
         echo `pwd`
      fi
      $0 `pwd`/"$i" "$2" $3 $4
   fi
done

if test $mode -eq 1 -a ! -d "$2"
then
   mkdir "$2"
fi

exit 0
