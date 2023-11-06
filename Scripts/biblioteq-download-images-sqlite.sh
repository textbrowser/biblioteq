#!/usr/bin/env sh
# Alexis Megas.

if [ ! -x "$(which base64)" ]; then
    echo "Missing base64."
    exit 1
fi

if [ ! -x "$(which sqlite3)" ]; then
    echo "Missing sqlite3."
    exit 1
fi

if [ ! -x "$(which wget)" ]; then
    echo "Missing wget."
    exit 1
fi

for i in "$@"; do
    if [ "$i" = "--help" ]; then
	echo "$0:\n" \
	     " --database file-name\n" \
	     " --ignore-not-null (download missing images only)\n" \
	     " --open-library (download from Open Library, default Amazon)\n" \
	     " --help"
	exit 0
    fi
done

amazon=1
file=""
query="SELECT TRIM(id, '-') FROM book WHERE id IS NOT NULL"

for i in "$@"; do
    if [ "$i" = "--database" ]; then
	file="1"
	continue
    fi

    if [ "$i" = "--ignore-not-null" ]; then
	query="SELECT TRIM(id, '-') FROM book WHERE front_cover IS NULL AND \
	       id IS NOT NULL"
    fi

    if [ "$i" = "--open-library" ]; then
	amazon=0
    fi

    if [ "$file" = "1" ]; then
	file="$i"
    fi
done

if [ ! -w "$file" ]; then
    echo "Please specify a writable database file."
    exit 1
fi

rm -f "$0.output"

if [ $amazon -eq 1 ]; then
    echo "Downloading images from Amazon."
else
    echo "Downloading images from OpenLibrary."
fi

for id in $(sqlite3 "$file" "$query"); do
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
    sqlite3 "$file" < "$0.output" 2>/dev/null

    if [ $? -eq 0 ]; then
	echo "Database $file processed correctly."
    else
	echo "Error processing $file."
    fi
fi

rm -f "$0.output"
