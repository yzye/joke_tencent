#!/bin/sh

cd ../
base_dir=`pwd | xargs -i basename {}`
#base_dir=lcs_new
if [ "$base_dir" == "lcs_new" ];then
    process=lcs
else
    process=$base_dir
fi

cd op/
ps -flu ${LOGNAME} 2>/dev/null | awk '{ if( FNR == 1 ) printf "%s\n", $0;}'
ps -flu ${LOGNAME} 2>/dev/null | grep "./${process}_ccd ../etc/${process}_ccd.conf" | grep -v grep | awk '{printf "%s\n", $0;}'
ps -flu ${LOGNAME} 2>/dev/null | grep "./${process}_mcd" | grep -v grep | awk '{printf "%s\n", $0;}'
ps -flu ${LOGNAME} 2>/dev/null | grep "./${process}_dcc" | grep -v grep | awk '{printf "%s\n", $0;}'

#ps -flu ${LOGNAME} 2>/dev/null | grep "./${process}_dcc_asn ../etc/${process}_dcc_asn.conf" | grep -v grep | awk '{printf "%s\n", $0;}'
exit 0
