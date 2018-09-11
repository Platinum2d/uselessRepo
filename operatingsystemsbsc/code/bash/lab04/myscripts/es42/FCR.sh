
#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri

#se è il caso, opero sui file con un ciclo esterno a quello delle directory. Altrimenti: usare quello standard
F="$2"
N=$3
tmp=$4

#utilizzo una variabile apposita per ricordarmi se ho già stampato il direttorio su standard output o meno (si evita così di stampare più volte il direttorio nel caso si trovino più file conformi)
saved=0

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      $0 `pwd`/"$i" "$2" $3 $4
   fi
   
   #controllo che l'elemento in questione sia un file e sia leggibile
   if test -f "$i" -a -r "$i"
   then
      case "$i" in
         *"$F") #controllo che la dimensione del file sia strettamente maggiore a N
                if test `wc -c < "$i"` -gt $N
                then
                    #memorizzo il file nel file temporaneo e stampo l'indirizzo assoluto direttorio, se non l'ho già fatto
	            echo "$i" >> $tmp
                    cat "$i" >> SOMMA
                    if test $saved -eq 0
	            then
                       saved=1
	               echo `pwd`
	            fi
                 fi;;
      esac
   fi
done

exit 0
