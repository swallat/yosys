
if (NOT SMALL)
    set(SOURCE_FILES ${SOURCE_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/synth.cc
        ${CMAKE_CURRENT_LIST_DIR}/prep.cc
        )
endif(NOT SMALL)

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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/simlib_help.inc
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/cellhelp.py ${CMAKE_CURRENT_LIST_DIR}/simlib.v > ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/simlib_help.inc
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/cellhelp.py ${CMAKE_CURRENT_LIST_DIR}/simlib.v
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMENT "Generating techlibs/common/simlib_help.inc"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/simcells_help.inc
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/cellhelp.py ${CMAKE_CURRENT_LIST_DIR}/simcells.v > ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/simcells_help.inc
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/cellhelp.py ${CMAKE_CURRENT_LIST_DIR}/simcells.v
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMENT "Generating techlibs/common/simcells_help.inc"
)

set(SOURCE_FILES ${SOURCE_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/simlib_help.inc
    ${CMAKE_CURRENT_BINARY_DIR}/techlibs/common/simcells_help.inc)

copy_files("${CMAKE_CURRENT_LIST_DIR}/*.v;${CMAKE_CURRENT_LIST_DIR}/*.lib" ${CMAKE_CURRENT_BINARY_DIR}/share/ copy-techlib-common-files)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/simlib.v DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/simcells.v DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/techmap.v DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/pmux2mux.v DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/adff2dff.v DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/dff2ff.v DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cells.lib DESTINATION ${CMAKE_INSTALL_DATADIR})
