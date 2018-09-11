#!/bin/sh
#FCR.sh
#descrizione dei parametri

cd "$1"
X="$2"
tmpfile="$3"

#chiamata ricorsiva fissa su ogni directory. La parte di elaborazione personalizzata puoò essere messa sia qui che dentro al for: dipende dal problema

for i in *
do	
   #controllo che la i corrente sia un file e che sia leggibile 
   if test -f "$i" -a -r "$i"
   then
     #variabile per contare il numero di linee che soddisfano le condizioni date
     c=0

     #prelevo il suo contenuto e comincio a scorrerne le righe 
     filerows=`wc -l < $i`
     j=1
     while test $j -le $filerows
     do
	cont=`head -$j $i | tail -1`
        case $cont in
           *t) c=`expr $c + 1` ;;
	esac

        j=`expr $j + 1`
     done

     #verifico che il numero di occorrenze all'interno del file sia maggiore di X
     if test "$c" -ge "$X"
     then
         echo `pwd`"/$i" >> $tmpfile
     fi
   fi

   if test -d "$i" -a -x "$i"
   then
      "$0" "$i" "$2" "$3" #parametri 
   fi
done

exit 0
