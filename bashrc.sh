#!/bin/bash
CURDIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$CURDIR/CDTE/lib"
