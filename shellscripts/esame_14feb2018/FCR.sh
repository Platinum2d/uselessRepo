
#!/bin/sh
#descrizione dei parametri

#mi sposto nella cartella prossima alla ricorsione e acquisisco i parametri
cd "$1"

#acquisisco i parametri
size="$2"
tmpnames="$3"

#scorro il contenuto della directory corrente
for i in *
do
   if test -d "$i" -a -x "$i"
   then
      "$0" "$i" "$size" "$3"
   fi

   #controllo che ciò che sto correntemente analizzando sia un file leggibile
   if test -f "$i" -a -r "$i"
   then
      if test `wc -l < "$i"` -eq $size
      then
         echo `pwd`
         echo "$i" >> "$tmpnames"
      fi
   fi
done

exit 0
