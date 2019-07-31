#!/bin/sh

if [ 1 != $# ]; then
  echo "usage: $0 type(debug|release)"
  exit
fi

blade build --generate-dynamic -p$1
