cd ../
#pn=`pwd | xargs -i basename {}`
pn=lcs
cd ./bin
kill -15 `cat $pn"_dcc.pid"` > /dev/null 2>&1
./$pn"_dcc" "../etc/"$pn"_dcc.conf"


kill -15 `cat $pn"_dcc2.pid"` > /dev/null 2>&1
./$pn"_dcc2" "../etc/"$pn"_dcc2.conf"
