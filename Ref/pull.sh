#!/bin/sh

STUDENTS="todoesverso moreandres mjtrangoni sebagalan rfgaraba caromontivero luisbosch ezequielsvelez ledusanchez"

for S in $STUDENTS; do
    echo $S
    cd $S
    git pull
    cd -
done
