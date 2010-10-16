#!/bin/bash


process_build_item()
{
    i=0
 
    target="$(echo ${1%\:})" # g
    echo $target

    firstchar=${target:0:2}
    fext_loc=`expr match "$target" '^.*\.'`
    fext=${target:$fext_loc:${#target}}
    echo "FEXT=${fext}"
    for object in "$@"
    do
        if [ $i != "0" ] 
        then
            echo  "Depends on: $object"
        fi

    i=$1+1
    done

    echo " "
}

# 
UNIXBUILDMAP=unixbuild.map

exec 3<&0
exec 0<"$UNIXBUILDMAP"

while read -r line
do
    if [ -n "$line" ]
    then
        process_build_item $line
    fi
done
exec 0<&3
exit 0
