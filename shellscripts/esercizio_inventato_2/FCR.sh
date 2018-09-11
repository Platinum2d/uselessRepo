
#!/bin/sh
#descrizione dei parametri

#mi sposto nella cartella prossima alla ricorsione e acquisisco i parametri
cd "$1"

#acquisisco i parametri
tmpfile="$2"

for i in *
do
   #se il contenuto corrente è una directory attraversabile, eseguo la chiamata ricorsiva su di essa
   if test -d "$i" -a -x "$i"
   then
      "$0" "$i" "$tmpfile" #parametri 
   fi

   #controllo che il contenuto corrente sia un file leggibile
   if test -f "$i" -a -r "$i"
   then 
      #controllo che il file abbia il formato descritto
      content=`cat "$i" 2>/dev/null`
      case "$content" in
         "#!/bin/"*) case "$content" in
		       *"exit 0") echo `pwd`/"$i" >> "$tmpfile";;
                    esac;;
      esac
   fi
done

exit 0
