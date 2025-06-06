#!/usr/bin/env bash
# SPDX-License-Identifier: MIT

VERSION=$1

if [ -z "$VERSION" ]
then
    echo "Please specify the version: $0 <VERSION>."
    exit 1
fi

# Debian release packages.

for FILE in Distributions/*/control
do
    sed -i "s/Version: .*/Version: $VERSION/" $FILE
done

# Version configuration.

FILE="Source/biblioteq.h"

sed -i 's/\(BIBLIOTEQ_VERSION "\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
sed -i 's/\(BIBLIOTEQ_VERSION_LTS "\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE

# Android manifest file.

FILE="Android/AndroidManifest.xml"

sed -i \
    's/\(android:versionCode="\)[0-9]\+\([0-9]\+\)*"/\1'"${VERSION//./}"'"/' \
    $FILE
sed -i \
    's/\(android:versionName="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
echo "Please remember README.md!"
echo "Please remember the release notes!"
