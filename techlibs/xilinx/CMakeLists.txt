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

set(SOURCE_FILES ${SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/synth_xilinx.cc
    )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/)
add_custom_target(generate-xilinx-files ALL
                  BYPRODUCTS
                  ${CMAKE_CURRENT_LIST_DIR}/brams_init_36.vh
                  ${CMAKE_CURRENT_LIST_DIR}/brams_init_32.vh
                  ${CMAKE_CURRENT_LIST_DIR}/brams_init_18.vh
                  ${CMAKE_CURRENT_LIST_DIR}/brams_init_16.vh
                  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/brams_init.py
                  COMMAND mv ${CMAKE_CURRENT_LIST_DIR}/brams_init_36.vh ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/
                  COMMAND mv ${CMAKE_CURRENT_LIST_DIR}/brams_init_32.vh ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/
                  COMMAND mv ${CMAKE_CURRENT_LIST_DIR}/brams_init_18.vh ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/
                  COMMAND mv ${CMAKE_CURRENT_LIST_DIR}/brams_init_16.vh ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/
                  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/brams_init.py
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Generating share/xilinx/brams_init_36.vh share/xilinx/brams_init_32.vh share/xilinx/brams_init_18.vh share/xilinx/brams_init_16.vh"
                  )

copy_files("${CMAKE_CURRENT_LIST_DIR}/*.v;${CMAKE_CURRENT_LIST_DIR}/*.txt" ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/ copy-xilinx-files)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} DESTINATION ${CMAKE_INSTALL_DATADIR} FILES_MATCHING PATTERN "*.v")
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} DESTINATION ${CMAKE_INSTALL_DATADIR} FILES_MATCHING PATTERN "*.txt")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/brams_init_36.vh DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/brams_init_32.vh DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/brams_init_18.vh DESTINATION ${CMAKE_INSTALL_DATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/xilinx/brams_init_16.vh DESTINATION ${CMAKE_INSTALL_DATADIR})
