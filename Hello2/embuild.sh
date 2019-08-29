#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

emcmake cmake ${DIR} -B ${DIR}/embuild
cd ${DIR}/embuild
emmake make



