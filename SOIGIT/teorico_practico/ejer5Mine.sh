#!/bin/bash

# Comprobar que se han proporcionado los tres parámetros requeridos
if [ $# -ne 3 ]; then
    echo "Faltan argumentos <matrícula> <ITV|Seguro|IVTM> <SI|NO>"
    exit 1
fi

# Asignar los parámetros a variables
matricula=$1
columna=$2
estatus=$3


#Aseguramos que la coolumna que se quiere ver sea correcto
if [ $columna != "ITV" -a $columna != "Seguro" -a $columna != "IVTM" ];then
    echo "El Segundo Argumento no es válido <ITV|Seguro|IVTM>"
    exit 1
fi

#Cogemos la linea entera del fichero coches (teniendo en cuenta minusculas y que sea exacto)"-i-w"
linea=$(grep -i -w "$matricula" coches.csv)

#Si "LINEA" es nulo, es que la matrícula no existe en el archivo
if [ -z "$linea" ]; then
    echo "Ese vehículo no se encuentra en la base de datos"
    exit 1
fi

# Actualizar el valor de la columna seleccionada en la línea correspondiente
if [ "$columna" = "ITV" ]; then
    valor=$(echo "$linea" | awk -F "," '{print $2}')
    
    if [ "$valor" = "$estatus" ]; then
        echo "El estatus de la $columna ya es $estatus para el vehículo $matricula"
        exit 0
    fi  
    linea_modificada=$(echo "$linea" | awk -v est=$estatus -F "," '{print $1","est","$3","$4}')

elif [ "$columna" = "Seguro" ]; then
    valor=$(echo "$linea" | awk -F ";"\|"," '{print $3}')
    
    if [ "$valor" = "$estatus" ]; then
        echo "El estatus de la $columna ya es $estatus para el vehículo $matricula"
        exit 0
    fi  
    linea_modificada=$(echo "$linea" | awk -v est=$estatus -F ";"\|"," '{print $1","$2","est","$4}')
else
    valor=$(echo "$linea" | awk -F ";"\|"," '{print $4}')
    
    if [ "$valor" = "$estatus" ]; then
        echo "El estatus de la $columna ya es $estatus para el vehículo $matricula"
        exit 0
    fi 
    linea_modificada=$(echo "$linea" | awk -v est=$estatus -F ";"\|"," '{print $1","$2","$3","est}')
fi


echo Lin: $linea
echo MOD_: $linea_modificada
# Actualizar el archivo coches.csv con la línea modificada
sed -i "s/$linea/$linea_modificada/" coches.csv

#echo "Estatus de la $columna actualizado para el vehículo $matricula"
exit 0
