#!/bin/bash
BACKUP=ADwiki.tar.gz
cd /home/dahee/ADwiki/
/bin/tar cvfz $BACKUP data > /dev/null
/bin/mv -f  $BACKUP /home/dahee/anydicdoc
#/bin/chown nobody.nobody $BACKUP
T=`/bin/date +%m%d%H`
cd /home/dahee/anydicdoc
/usr/local/svn/bin/svn -q ci $BACKUP -m "$BACKUP $T"

