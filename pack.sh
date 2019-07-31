#!/bin/bash

source ../../env.sh

if [ 1 != $# ]; then
  echo "usage: $0 type(debug|release)"
  exit
fi

mod_dir=`pwd`
mod_name="joke"
path_name=$proj_name"/sub_service/"$mod_name

ver=`get_version $path_name`
dst=$mod_name".r"$ver"."$1
file=$mod_name".r"$ver"."$1".tar.gz"

if [ -d $dst ]; then
  rm -rf $dst/*
else
  mkdir $dst
fi
gen_version_file $dst

cp -r deploy_dir/* $dst
cd $root_dir
find build64_$1 -name "*.so" | xargs -i cp --parents {} $mod_dir/$dst/bin
cp build64_$1/$proj_name/public/mcp++/* $mod_dir/$dst/bin
cd $mod_dir/$dst/bin
./make_links.sh $1
cd $mod_dir

if [ -f $file ]; then
  rm $file
fi

tar zcvf $file $dst/*
