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
	mv $i ~/BiblioteQ-"$VERSION"_arm64.apk
    fi

    if [[ "$i" == *"armeabi"* ]]
    then
	mv $i ~/BiblioteQ-"$VERSION"_arm32.apk
    fi
done
