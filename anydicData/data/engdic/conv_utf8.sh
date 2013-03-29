for i in ?.dic
do 
echo $i
iconv -c -f euc-kr -t utf-8 $i -o $i.utf8
done
