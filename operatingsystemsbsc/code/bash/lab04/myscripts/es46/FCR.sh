
#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri
lines="$2"
tempfile="$3"

#creo una variabile per verificare che la directory corrente sia già stata salvata nel file temporaneo onde evitare stampe ripetute
already_saved=0
num=1

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      "$0" "$i" "$2" "$tempfile"
   fi
   
   if test -f "$i"
   then
      #controllo che il file possieda il numero di linee desiderato
      if test `wc -l < "$i"` -eq "$lines"
      then
	 #controllo che l'indirizzo da salvare non sia già stato salvato in precedenza
         if test "$already_saved" -ne 1
	 then
            echo `pwd` >> "$tempfile"
	 fi

         #impongo la variabile flag a 1 per la ragione sopra riportata
         already_saved=1
         
         #creo il file sorted
         sort "$i" > "$i".sort
      fi
   fi
done

exit 0
