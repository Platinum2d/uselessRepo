#!/bin/sh

#controllo che il numero dei parametri sia almeno 3 (uno per il nome del file e almeno due gerarchie)
case $# in
   0 | 1 | 2 ) echo "usage : $0 [simple file name] [absolute directory path 1] ... [simple directory path n]"
	       exit 1;;
esac

case $1 in
   */*) echo "usage : $1 [simple file name] [absolute directory path 1] ... [simple directory path n]"
	exit 2;;
esac

#memorizzo il nome del file e faccio uno shift a sinistra dei parametri, così da poter trattare solo le gerarchie passate
f=$1
shift

#controllo che ogni gerarchia sia un indirizzo assoluto, una directory e che sia accessibile
for i in $*
do
   case $i in
      /*) if test ! -d $i  
          then
	     echo "error: $i is not a directory"
	     exit 3
          else
	     if test ! -x $i
	     then
	        echo "error: $i is not readable"
		exit 4
	     fi
          fi;;
      *) echo "usage : $0 [simple file name] [absolute directory path 1] ... [simple directory path n]"
        exit 5;;
   esac
done

#modifico la variabile di ambiente PATH per facilitare il processo ricorsivo in FCR.sh
PATH=`pwd`:$PATH
export PATH

#assegno un nome al file temporaneo
tmpname=/tmp/cont

#creo un file temporaneo per contenere il numero di file sorted creati
touch $tmpname

#eseguo il singolo file FCR.sh per ognuna delle gerarchie passate come parametro
for i
do
   FCR.sh $f $i $tmpname
done

#stampo il numero di file creati
echo "Sono stati creati " `wc -l < $tmpname` " file" 

#scorro il file temporaneo e stampo i contenuti richiesti per ogni sua linea
for line in `cat $tmpname`
do
   echo "[F] $line"
   echo "prima linea: "`head -1 < $line`
   echo "ultima linea: "`tail -1 < $line`
   echo ""
done

#elimino il file precedentemente creato
rm $tmpname
exit 0
