#!/bin/bash

if [ $# -ne 1 ]
then 
    echo "Nombre incorrecte de parametres"
    exit 1
fi

if [ ! -d $1 ]; then
    echo "El argumento no es un directorio"
fi

ls -l $1 >> temp.txt

fitxer=temp.txt

bytes=($(awk '{print $5}' $fitxer))

len=${#bytes}


i=0
sum=0
while [ $i -lt $len ];
do
  sum=$(($sum+${bytes[$i]}))
    ((i++))
done


rm temp.txt
echo $sum

exit 0
