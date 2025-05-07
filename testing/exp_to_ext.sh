#~/bin/bash

## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

convert_exp_to_ext () {
    echo -e "// DO NOT EDIT! THIS FILE IS GENERATED!!!!!!!!!!!!!!!!!\n\n$(cat $1)" > $2
    sed -i 's/ZE_experimental_rtas_builder/ZE_extension_rtas/g' $2
    sed -i 's/rtas\([[:alnum:]_]*\)exp/rtas\1ext/g' $2
    sed -i 's/RTAS\([[:alnum:]_]*\)EXP/RTAS\1EXT/g' $2
    sed -i 's/EXP\([[:alnum:]_]*\)RTAS/EXT\1RTAS/g' $2
    sed -i 's/RTAS\([[:alnum:]_]*\)Exp/RTAS\1Ext/g' $2
    sed -i 's/EXP_API/EXT_API/g' $2
}

convert_exp_to_ext rthwif_cornell_box.cpp rthwif_cornell_box_ext.cpp
convert_exp_to_ext rthwif_test.cpp rthwif_test_ext.cpp
