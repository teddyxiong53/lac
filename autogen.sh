#!/bin/sh
autoreconf --verbose --force --install --make || {
 echo 'autogen.sh failed';
 exit 1;
}

echo
echo "Now you can 'configure' then 'make'"
echo


