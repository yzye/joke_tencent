cd ../
#pn=`pwd | xargs -i basename {}`
pn=gsc_qa
cd ./bin
kill -15 `cat $pn"_watchdog.pid"` > /dev/null 2>&1
./$pn"_watchdog"
