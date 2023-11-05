#!/usr/bin/env sh
# Alexis Megas.

if [ ! -x "$(which base64)" ]; then
    echo "Missing base64."
    exit 1
fi

if [ ! -x "$(which psql)" ]; then
    echo "Missing pqsql."
    exit 1
fi

if [ ! -x "$(which wget)" ]; then
    echo "Missing wget."
    exit 1
fi

if [ ! "$#" -ne 2 ]; then
   echo "Syntax: $0 database-account database-name"
   exit 1
fi

rm -f "$0.output"

for id in \
    $(psql -U $1 \
	   -W \
	   -c 'SELECT id FROM book WHERE id IS NOT NULL' \
	   -d $2 \
	   -t \
	   --csv); do
    /bin/echo -n "Fetching $id's image... "
    wget --output-document "$id.jpg" \
	 --quiet \
	 "https://m.media-amazon.com/images/P/$id.01._SCMZZZZZZZ_.jpg"

    if [ $? -eq 0 ]; then
	echo "Image downloaded."
	echo "UPDATE book SET front_cover = " \
	     "'$(cat "$id.jpg" | base64)'" \
	     "WHERE TRIM(id, '-') = " \
	     "'$id';" >> "$0.output"
    else
	echo "Image not found."
    fi

    rm -f "$id.jpg"
done

rm -f "$0.output"
