#echo $ADHOME
/export/home/anydict/ad2005

* ./testExtract : extract.cpp -- main() 
./testExtract 
usage: option
        db
        edict
        extract
        search
 
# ./testExtract  extract  
 --> 영어문장 테스트:  "txt/www_whitehouse_gov-_news_releases_2005_04_20050421-4.txt"  --> $ADHOME/txt/.....
 
# ./testExtract  extract  

 
### 주의 -- 한글 lib 을 만들어야 한다.
# anydict@zetamobile.com /export/home/anydict/ad2005/hangul > make -f Makefile.lib 
#

./testExtract extract extract/h.txt
