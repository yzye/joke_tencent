cd ../
#pn=`pwd | xargs -i basename {}`
pn=lcs
cd ./bin
kill -15 `cat $pn"_dcc.pid"`
kill -15 `cat $pn"_dcc2.pid"`
