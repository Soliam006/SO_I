#!/bin/bash

#!/bin/bash

if [ $# -ne 1 ];
then
    echo "Nombre incorrecto de parametros $0 <dir> <num>"
    exit 1
fi

if [ ! -f $1 ]; 
then
    echo "El fichero $1 no existe"
    exit 1
fi

directorios=$(cat $1)


noExiste=0
existe=0

for i in $directorios 
do
    if [ -f $i ];
    then
        existe=$(($existe+1))
    else
        noExiste=$(($noExiste+1))
    fi
done

echo "Existen: $existe"
echo "No existen: $noExiste"
exit 0