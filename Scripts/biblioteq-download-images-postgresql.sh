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

for i in "$@"; do
    if [ "$i" = "--help" ]; then
	echo "$0:\n" \
	     " --account database account\n" \
	     " --database database name\n" \
	     " --ignore-not-null (download missing images only)\n" \
	     " --open-library (download from Open Library, default Amazon)\n" \
	     " --help"
	exit 0
    fi
done

account=""
amazon=1
database=""
query="SELECT TRIM(id, '-') FROM book WHERE id IS NOT NULL"

for i in "$@"; do
    if [ "$account" = "1" ]; then
	account="$i"
    fi

    if [ "$database" = "1" ]; then
	database="$i"
    fi

    if [ "$i" = "--account" ]; then
	account="1"
	continue
    fi

    if [ "$i" = "--database" ]; then
	database="1"
	continue
    fi

    if [ "$i" = "--ignore-not-null" ]; then
	query="SELECT TRIM(id, '-') FROM book WHERE front_cover IS NULL AND \
	       id IS NOT NULL"
    fi

    if [ "$i" = "--open-library" ]; then
	amazon=0
    fi
done

rm -f "$0.output"
echo "Please provide the PostgreSQL password for $account."

for id in \
    $(psql -U "$account" \
	   -W \
	   -c 'SELECT id FROM book WHERE id IS NOT NULL' \
	   -d "$database" \
	   -t \
	   --csv); do
    /bin/echo -n "Fetching $id's image... "

    if [ $amazon -eq 1 ]; then
	wget --output-document "$id.jpg" \
	     --quiet \
	     "https://m.media-amazon.com/images/P/$id.01._SCMZZZZZZZ_.jpg"
    else
	wget --output-document "$id.jpg" \
	     --quiet \
	     "https://covers.openlibrary.org/b/isbn/$id-L.jpg"
    fi

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

if [ -r "$0.output" ]; then
    if [ $? -eq 0 ]; then
	echo "Database $database processed correctly."
    else
	echo "Error processing $database."
    fi
fi

rm -f "$0.output"
