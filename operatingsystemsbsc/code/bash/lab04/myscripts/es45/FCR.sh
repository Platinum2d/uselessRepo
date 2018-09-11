
#!/bin/sh

#mi sposto nella directory da analizzare
cd "$1"

#acquisisco i parametri
tmpch="$2"
tmpfile="$3"

#se è il caso, opero sui file con un ciclo esterno a quello delle directory. Altrimenti: usare quello standard

#scorro il contenuto della cartella in cui mi trovo attualmente
for i in *
do
   #effettuo la chiamata ricorsiva solo se sto analizzando una cartella
   if test -d "$i" -a -x "$i" 
   then
      "$0" "$i" "$2" "$3"
   fi
   
   if test -f "$i"
   then
      #ricavo la dimensione del file
      dim=`wc -c < "$i"`
      
      #echo "analizzo il file $i di dimensione $dim"
      #read pause
      #verifico che la dimensione sia pari
      ok=1
      if test `expr $dim % 2` -eq 0
      then
	  #echo "   la dimensione è pari: controllo se ci sono tutti i caratteri"
	  #read pause
          #verifico che ogni carattere sia contenuto all'interno del file
	  charlist=`cat "$tmpch"`
	  filecontent=`cat "$i"`
	  for c in $charlist
	  do
             case "$filecontent" in
	        *$c*);;
		*) ok=0;;
	     esac
	  done
	  #se la variable "booleana" ok è ancora a 1, significa che ha passato tutti i controlli, quindi il file contiene tutti i caratteri richiesti
	  if test "$ok" -eq 1
	  then
             #echo "il file è idoneo: lo registro"
	     #read pause
	     echo "[$dim bytes] "`pwd`/$i >> "$tmpfile"
	  fi
      fi
   fi
done

exit 0
