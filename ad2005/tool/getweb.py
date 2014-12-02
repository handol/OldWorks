import urllib, sys, time
#web = urllib.urlopen("http://www.anydic.com")

#web = urllib.urlopen("http://127.0.0.1:8010/harvest?expr=http://www.anydict.com")


def test_harv(i=0):
    web = urllib.urlopen("http://127.0.0.1:8010/harvest?expr=http%3A%2F%2Fwww.anydict.com&cnt=" + str(i))
    #print web.info()
    #print web.read()



if __name__ == "__main__":
    for i in range(1000):
        print i
        test_harv(i)
        time.sleep(0.02)

