#!/bin/sh
if test $# -ne 3 
then
  echo "usage: $0 [absolute directory path] [simple directory name] [positive integer]"
  exit 1
fi

case $1 in
   /*) case $2 in
          */*) echo "usage: $0 [absolute directory path] [simple directory name] [positive integer]"
	       exit 2;; 
       esac

       expr $3 + 0 > /dev/null 2>&1 
       case $? in 
          0 | 1) ;;
	  2) echo "usage: $0 [absolute directory path] [simple directory name] [positive integer]" 
	     exit 3;;
	  3) echo "usage: $0 [absolute directory path] [simple directory name] [positive integer]" 
             exit 4;;
       esac
       if test $3 -le 0
       then
          echo "usage: $0 [absolute directory path] [simple directory name] [positive integer]"
	  exit 5
       fi;;
       
   *)  echo "usage: $0 [absolute directory path] [simple directory name] [positive integer]" 
       exit 6;;
esac

PATH=$PATH:`pwd`
export PATH

touch /tmp/cont
echo "0" > /tmp/cont
FCR.sh $1 $2 $3
echo "Numero di file che violano: `cat /tmp/cont`"
rm -r /tmp/cont
exit 0
