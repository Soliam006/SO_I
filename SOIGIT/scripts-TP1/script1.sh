#!/bin/bash

if [ $# -ne 2 ]
then 
    echo "Nombre incorrecte de parametres: $0 <num1> <num2>"
    exit 1
fi

A=$1
B=$2 

if [ $A -gt $B ]
 then
  echo "$A es major que $B"
  exit 0
 else
  echo "$A es menor que $B"
  exit 1
fi 

