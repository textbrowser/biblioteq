#!/usr/bin/env bash

# Alexis Megas.

VERSION=$1

if [ -z "$VERSION" ]
then
    echo "Please specify the version: $0 <VERSION>."
    exit 1
fi

for i in $(find . -iname '*.apk')
do
    if [[ "$i" == *"arm64"* ]]
    then
	filename="~/BiblioteQ-"$VERSION"_arm64.apk"

	mv $i "$filename"

	if [ $? -eq 0 ]
	then
	    echo "Created the file $filename."
	else
	    echo "Could not create $filename."
	fi
    fi

    if [[ "$i" == *"armeabi"* ]]
    then
	filename="~/BiblioteQ-"$VERSION"_arm32.apk"

	mv $i "$filename"

	if [ $? -eq 0 ]
	then
	    echo "Created the file $filename."
	else
	    echo "Could not create $filename."
	fi
    fi
done
