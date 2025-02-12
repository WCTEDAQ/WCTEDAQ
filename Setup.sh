#!/bin/bash

#Application path location of applicaiton

Dependencies=`pwd`/Dependencies

source `pwd`/Dependencies/root_v6.24.08/bin/thisroot.sh

export LD_LIBRARY_PATH=`pwd`/lib:${Dependencies}/zeromq-4.0.7/lib:${Dependencies}/boost_1_66_0/install/lib:${Dependencies}/ToolFrameworkCore/lib:${Dependencies}/ToolDAQFramework/lib:${Dependencies}/caen/lib:$LD_LIBRARY_PATH

export SEGFAULT_SIGNALS="all"
