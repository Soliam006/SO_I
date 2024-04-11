
#!/bin/bash
if [ $# -lt 1 ]
then
    echo "Nombre de parametres incorrecte"
    exit 1
fi
longest=0

for word in $(grep -i -o '[a-z]*' "$1")
do
    len=${#word}
    if [ $len -gt $longest ]
        then
        longest=$len
        longword=$word
    fi
done

echo $1 $longest $longword