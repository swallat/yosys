#
#   yosys -- Yosys Open SYnthesis Suite
#
#   Copyright (C) 2017  Sebastian Wallat <sebastian.wallat@rub.de>
#
#   Permission to use, copy, modify, and/or distribute this software for any
#   purpose with or without fee is hereby granted, provided that the above
#   copyright notice and this permission notice appear in all copies.
#
#   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

include(${CMAKE_CURRENT_LIST_DIR}/hierarchy/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/cmds/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/proc/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/opt/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/techmap/CMakeLists.txt)

if(NOT SMALL)
    include(${CMAKE_CURRENT_LIST_DIR}/equiv/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/fsm/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/memory/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/sat/CMakeLists.txt)
    include(${CMAKE_CURRENT_LIST_DIR}/tests/CMakeLists.txt)
endif(NOT SMALL)