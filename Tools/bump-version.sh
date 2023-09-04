#!/usr/bin/env bash
# SPDX-License-Identifier: MIT

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: $0 <VERSION>"
    exit 1
fi

# Debian release packages.

for FOLDER in Distributions/DEBIAN \
	      Distributions/DEBIAN-POWERPC \
	      Distributions/RASPBIAN \
	      Distributions/PIOS64; do
    sed -i "s/Version: .*/Version: $VERSION/" $FOLDER/control
done

# Version configuration.

FILE="Source/biblioteq.h"

sed -i 's/\(BIBLIOTEQ_VERSION "\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' $FILE

# Android manifest file.

FILE="android/AndroidManifest.xml"

sed -i \
    's/\(android:versionName="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
sed -i \
    's/\(android:versionCode="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"${VERSION//./}"'"/' \
    $FILE
