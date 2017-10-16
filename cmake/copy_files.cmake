macro(copy_files GLOBPAT DESTINATION TARGET_NAME)
    file(GLOB_RECURSE COPY_FILES
         RELATIVE ${CMAKE_CURRENT_LIST_DIR}
         ${GLOBPAT})
    add_custom_target(${TARGET_NAME} ALL
                      COMMENT "Copying files: ${GLOBPAT}")
    message(STATUS "COPY_FILES: ${COPY_FILES}")
    foreach(FILENAME ${COPY_FILES})
        set(SRC "${CMAKE_CURRENT_LIST_DIR}/${FILENAME}")
        set(DST "${DESTINATION}/${FILENAME}")

        add_custom_command(
                TARGET ${TARGET_NAME}
                COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
        )
    endforeach(FILENAME)
endmacro(copy_files)