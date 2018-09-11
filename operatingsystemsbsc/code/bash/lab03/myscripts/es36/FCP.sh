#!/bin/sh
if test $# -ne 2 
then
  echo "usage: $0 [absolute directory] [simple filename]"
  exit 1
fi

case "$1" in
   /*) if test ! -d "$1"
       then
          echo "usage: $0 [absolute directory] [simple filename]" 
	  exit 2
       fi;;
   *)  echo "usage: $0 [absolute directory] [simple filename]" 
       exit 3;;
esac

PATH=$PATH:`pwd`
export PATH

FCR.sh $1 $2
exit 0
