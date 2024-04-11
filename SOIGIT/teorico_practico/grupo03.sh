#!/bin/bash

if [ $# -ne 1 ]                                                     #Comprovem que s'han passat l'únic argument, el directori
then
    echo "Nombre incorrecte de parametres: $0 <directori> "
    exit 1                                                          #En cas que falten o sobren arguments, mostrem el missatge d'error i sortim amb el codi d'error 1
fi

directori=$1                                                        #Assignem el valor del directori a una variable més descriptiva
if [ ! -d $directori ]                                              #Comprovem que és un directori
then                               
    echo "Error: el directori $directori no existeix"
    exit 1                                                          #En cas que no sigui un directori, mostrem el missatge d'error i sortim amb el codi d'error 1
fi


directoris=$(find "./"$directori -type d )                          #Obtenim tots els directoris que conté aquest directori i els seus subdirectoris
for subdirectori in $directoris                                     #Recorrem tots els directoris
do
        count=$(find $subdirectori -maxdepth 1 -type f | wc -l)     #Contem tots els fitxers que conté el subdirectori
        echo "directorio: $subdirectori nfiles: $count"             #Imprimim el nombre de fitxers que té cada subdirectori
done

exit 0                                                              #Si el programa s'ha completat correctament sortim amb exit 0
