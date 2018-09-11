
#!/bin/sh

#controllo che il numero dei parametri sia pari a 2
if test $# -ne 2
then
   echo " Utilizzo : $0 gerarchia direttorio"
   exit 1
fi

#controllo che il primo parametro sia espresso in forma assoluta
case $1 in
   /*) if test ! -d $1
       then
          echo " Errore : $1 non è una directory"
	  exit 2
       else
	  if test ! -x $1
	  then
	      echo " Errore : $1 non è accessibile"
	      exit 3
          fi
       fi 
	   ;;
   *) echo " Errore : $1 non è un nome assoluto di directory"
       exit 4;;
esac	

#controllo che il secondo parametro sia un nome relativo semplice
case "$2" in
   */*) echo " Errore : $2 non è un nome relativo semplice"
        exit 5;;
   *) #eventuali altri controlli 
      ;;
esac

#creo il file temporaneo
tempfile=/tmp/t$$
touch $tempfile

#aggiorno la variabile d'ambiente PATH per facilitare il processo ricorsivo
PATH=`pwd`:$PATH
export PATH

#utilizo una variabile apposita per distinguere le modalità operative del file
mode=0

echo ""
echo "Direttori trovati:"
#chiamata al file ricorsivo
cd $1
for i in *
do
   #controllo che il direttorio all'interno della radice sia accessibile. Vengono usate le virgolette per scongiurare errori dovuti alla presenza di wildcard nel nome dei file/direttori
   if test -d "$i" -a -x "$i"
   then
      FCR.sh "$i" "$2" $tempfile $mode
   fi
done

#chiedo all'utente se vuole eseguire la fase B
echo -n "Si desidera creare il direttorio $2 ove non esiste? (y/altro) : "
read sc
case "$sc" in
   y | Y) mode=1 
	  for i in *
	  do
             if test -d "$i" -a -x "$i"
	     then
	        FCR.sh "$i" "$2" $tempfile $mode
	     fi
	  done
	  
	  echo ""
	  echo "Verifica della creazione dei direttori"
	  echo ""
          for i in *
          do
             if test -d "$i" -a -x "$i"
             then
		mode=0
                FCR.sh "$i" "$2" $tempfile $mode
             fi
          done;;
   *) ;;
esac

#elimino il file temporaneo
rm $tempfile

exit 0
