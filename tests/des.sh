#!/bin/bash

MAX_MSG=24
LOOPS=100

for i in $(seq 1 ${LOOPS}) ; do
  MSG=$(openssl rand -hex $((1 + $RANDOM % ${MAX_MSG})))
  KEY=$(openssl rand -hex 8)
  echo "$i KEY=${KEY} MSG=${MSG}"
  LUA=$(lua des.lua encrypt ${MSG} ${KEY})
  SSL=$(echo -n ${MSG} | xxd -r -p | openssl enc -e -des-ecb  -K ${KEY} -provider legacy | xxd -p -c 0)
  if [[ "${LUA}" != "${SSL}" ]] ; then
    echo "ENCRYPTION ERROR: KEY=${KEY} MSG=${MSG}"
    echo "${LUA} != ${SSL}"
    exit -1
  fi
  MSG="${LUA}"
  LUA=$(lua des.lua decrypt ${MSG} ${KEY})
  SSL=$(echo -n ${MSG} | xxd -r -p | openssl enc -d -des-ecb  -K ${KEY} -provider legacy | xxd -p -c 0)
  if [[ "${LUA}" != "${SSL}" ]] ; then
    echo "DECRYPTION ERROR: KEY=${KEY} MSG=${MSG}"
    echo "${LUA} != ${SSL}"
    exit -1
  fi
done

echo "SUCCES"

exit 0
