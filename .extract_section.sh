#! /bin/bash

while getopts f:v: flag
do
    case "${flag}" in
        f) filename=${OPTARG};;
        v) version=${OPTARG};;
    esac
done

if [ -z "$filename" ]; then
    echo "Please specify a filename using '-f'"
    exit 1
fi

if [ ! -f $filename ]; then

    echo "File does not exist: ${filename}"
    exit 1

fi

if [ -z "$version" ]; then
    echo "Please specify a version using '-v'"
    exit 1
fi

#echo "Analyzing file $filename"
header="## ${version}"
#echo "Section title: <${header}>"

foundTitle=false
while read line; do
    if [ "$foundTitle" == false ]; then
        if [[ "$line" =~ ^${header} ]]; then
            foundTitle=true
        fi
    else
        if [[ $line = '## '* ]]; then
            break
        else
            echo "$line"
        fi
    fi
done <$filename

if [ "$foundTitle" == false ]; then
    echo "[No changes were documented]"
fi
