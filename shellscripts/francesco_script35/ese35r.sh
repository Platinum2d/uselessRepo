#!/bin/sh

cd $1

#Recursive cycle

for file in *; do
	if test -d "$file" -a -x "$file"; then
	   $0 "$file" "$D" "$N" 
	   n=$( expr $n + $? )
	fi
done

#Scorro tutti i file del direttorio in cui mi trovo: se trovo un direttorio con lo stesso nome, conto il numero di file che hanno
# dimensione maggiore di N
DIR=$file
n=0

case $DIR in 
    $D) nome_ok=1

      echo "direttorio trovato"
#sono nel direttorio di nome D; ora scorro tutti i file di questo direttorio e vedo se c'è almeno un file di dimensione in byte > N
          for fname in *; do
	    if test -f $fname -a -x $fname; then 
	    fsize=`cat $fname | wc -c`
	       if test $fsize -gt $N; then
	       n=`expr $n + 1`
	       list="$list $fname"
               fi
            fi
#controllo finale: se il direttorio si chiama D e c'è almeno un file di dimensione maggiore di n, stampo il percorso
	 if test $n -ge 1 -a $nome_ok -eq 1; then
	    echo "[found]! `pwd`"
	    cat $list > SOMMA
         fi
        done 
	;;
     *) continue ;;
esac

exit $n
