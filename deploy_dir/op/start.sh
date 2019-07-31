#!/bin/bash
# Check the ulimit settings
if [ `cat ~/.bashrc | grep "ulimit -c unlimited" | wc -l` -lt 1 ]; then
    echo "ulimit -c unlimited" >> ~/.bashrc
fi

source ~/.bashrc

# Check crontab
path=`pwd`
crontab -l > crontab.tmp
if [ `cat crontab.tmp | grep "${path}" | grep "check.sh" | grep -v "^#" | wc -l` -lt 1 ]; then
    echo "*/1 * * * * cd ${path}; ./check.sh >> monitor.log 2>&1 &" >> crontab.tmp
    crontab crontab.tmp
fi
rm crontab.tmp

sh clear_mq.sh

cd ../
pn=`pwd | xargs -i basename {}`
#pn=gsc_qa
wd=`grep watchdog_conf_file "./etc/"$pn"_ccd.conf"|grep -v '#'|wc -l`
cd ./op
if [ $wd -eq 0 ]
then
	./start_ccd.sh
	./start_mcd.sh
else
	./start_watchdog.sh
fi
