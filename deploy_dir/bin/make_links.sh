#!/bin/sh

if [ $# -ne 1 ]; then
    echo "$0 type(debug|release)"
    exit -1
fi

ccd_path="ccd"
mcd_path="mcd"
dcc_path="dcc"

#create busi_logic ccd
unlink joke_ccd
ln -sf ${ccd_path} joke_ccd
unlink joke_mcd
ln -sf ${mcd_path} joke_mcd

#link admin and mcd so
unlink libjoke.so
ln -sf build64_$1/nlu/sub_service/joke/libjoke.so

#complte check so
unlink libnet_complete.so
ln -sf build64_$1/nlu/sub_service/joke/libnet_complete.so

