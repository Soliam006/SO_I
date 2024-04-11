#!/bin/bash

if [ $# -ne 1 ]
then 
    echo "Nombre incorrecte de parametres"
    exit 1
fi

fitxer=$1
col1=($(awk '{print $1}' $fitxer)) 
col2=($(awk '{print $2}' $fitxer))

len=${#col1[*]}

i=0
sum1=0
sum2=0

while [ $i -lt $len ]
do
  sum1=$(($sum1+${col1[$i]}))
  sum2=$(($sum2+${col2[$i]}))
  (( i++ ))
done
echo $sum1 $sum2

exit 0
