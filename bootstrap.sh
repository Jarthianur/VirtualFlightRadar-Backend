#!/bin/bash

# name of the executable, service (default: vfrb)
# example: VFRB
export VFRB_NAME_B=

# name of .properties file (default: vfrb.properties)
# file extension will be appended automatically
# example: VFRB
export VFRB_PROP_B=

# compiler to use (default: g++)
# may be set to path to cross compiler
export VFRB_COMPILER_B=

# set only if boost installed in custom directory
# example: /home/user/boost/
export BOOST_ROOT_B=

# path to executable after installation (default: target/)
# example: /home/user/bin/
export VFRB_EXEC_PATH_B=

# path to .properties file (default: target/)
# example: /home/user/
export VFRB_PROP_PATH_B=