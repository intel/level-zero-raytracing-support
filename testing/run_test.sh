#!/bin/bash
set -e
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:.
$*
