#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

#todo use separate components to prevent requiring
# libraries that aren't used on a specific setup

if (${EMSCRIPTEN})
  #todo maybe specify boost and other include directories??
else (${EMSCRIPTEN})
endif (${EMSCRIPTEN})
