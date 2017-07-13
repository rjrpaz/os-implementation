#!/bin/sh

STUDENTS="todoesverso moreandres mjtrangoni sebagalan rfgaraba caromontivero luisbosch ezequielsvelez ledusanchez"

for S in $STUDENTS; do
    echo $S
    git clone git://github.com/$S/os-implementation.git $S
done
