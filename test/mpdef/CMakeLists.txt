#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

add_executable(test_core
	main.cpp
	factorial.cpp
	path.cpp
	variant.cpp
	store.cpp
	context_read.cpp
	#msg_id.cpp #requires boost::uuid
	)

#bleh
target_include_directories(test_core
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../src"
  )
