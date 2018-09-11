#!/bin/sh

#acquisco i parametri
dir="$1"
cx="$2"
cy="$3"
tmp="$4"

#mi sposto nella cartella da analizzare
cd "$dir"

#utilizzo una variabile per non memorizzare la stessa cartella più volte
added=0

#scorro il contenuto della directory attuale
for file in *
do
   #effettuo la chiamata ricorsiva sul file solamente se è una directory attraversabile
   if test -d "$file" -a -x "$file"
   then
      "$0" "$file" "$2" "$3" "$4"
   fi

   #analizzo il file solo se è leggibile e solo se non ho già analizzato la cartella corrente
   if test -f "$file" -a -r "$file" -a "$added" -ne 1
   then
      #verifico che il nome del file contenga i caratteri Cx e Cy
      case "$file" in
         *$cx*$cy* | *$cy*$cx* ) ok_name=1;;
	 *) ok_name=0 ;;
      esac

      #verifico che il contenuto del file contenga i carattere Cx e Cy
      case `cat "$file"` in
         *$cx*$cy* | *$cy*$cx* ) ok_cont=1;;
	 *) ok_cont=0 ;;
      esac
      
      #se entrambi i pattern matching sono positivi, memorizzo nel file temporaneo l'indirizzo assoluto (avrei potuto anche stamparlo e basta, ma volevo anche stampare il numero di directories trovate come surplus)
      if test "$ok_name" -eq 1 -a "$ok_cont" -eq 1
      then
         echo "`pwd`" >> "$tmp"
	 #mi ricordo che la cartella l'ho memorizzata
	 added=1
      fi
   fi
done

exit 0
