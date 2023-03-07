#echo "" > $1
echo $1
tail -f $1 | sox -t ima -c 1 -r 16000 -v 7 - -d &
