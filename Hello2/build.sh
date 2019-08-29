#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cmake ${DIR} -B ${DIR}/build
cd ${DIR}/build
make


