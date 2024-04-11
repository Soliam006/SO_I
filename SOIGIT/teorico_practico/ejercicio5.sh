#!/bin/bash

#Comprovamos que el numero de parametros es correcto
if [ $# -ne 3 ]; then
    echo "Numero de parametros incorrecto: <matricula> <seguro> <SI/NO>"
    exit 1
fi

# Conseguimos la primera linea, donde esta el header del formato del archivo
PLinea=$(cat coches.csv | head -1 | awk -F "," '{print $1,$2,$3,$4}')

# Estas variables nos ayduaran a saber la posicion del seguro
contador=0
didBreak=false

# Iteramos por la primera linea hasta encontrar el seguro que corresponde
for i in $PLinea; do

    # Si encontramos el seguro hacemos un brea (nos quedamos con el contador) y
    # Señalamos que hemos hecho el break.
    if [ "$i" = "$2" ]; then
        echo $i
        didBreak=true
        break
    fi
    ((contador++))
done

# Mensaje de que no se ha encontrado el seguro
if [ ! $didBreak ]; then

    echo "El seguro no esta en la base de datos. <ITV | Seguro | IVTM >"
    exit 1
fi

# Reasignacion de variables
seguro=$2
estatus="$3"
matricula=$1
linea=$(grep -wi $1 coches.csv) # Conseguimos la linea de la matricula en question


if [ -z "$linea" ]; then
    echo "Este coche no existe en la base de datos."
    exit 1
fi

# Segun la posicion del seguro, cambiamos el estado del seguro reescribiendo la linea.
if [ $contador = 1 ]; then

    newLinea=$(echo $linea | awk -F "," "{print $1","($2="$estatus")"," $3","$4}")

elif [ $contador = 2 ]; then

    newLinea=$(echo $linea | awk -F "," "{print $1","$2"," ($3 = "$estatus")","$4}")

elif [ $contador = 3 ]; then

    newLinea=$(echo $linea | awk -F "," "{print $1","$2"," $3","($4 = "$estatus")}")

fi

# Reescribimos la nueva linea en el fichero.
sed -i "s/$linea/$newLinea/g" coches.csv

echo "El estatus de $2 modificado para el vehiculo $1" # Mensaje informativo.
exit 0