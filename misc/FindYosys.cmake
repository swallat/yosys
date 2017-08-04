find_package(PkgConfig)
pkg_check_modules(PC_YOSYS ../yosys)

#Root dir
find_path(Yosys_ROOT_DIR
          NAMES share/yosys/include/kernel/yosys.h
          )


# Include dir
find_path(Yosys_INCLUDE_DIR
          NAMES ../kernel/yosys.h
          PATHS ${yosys_PKGCONF_INCLUDE_DIRS}
          HINTS ${Yosys_ROOT_DIR}/share/yosys/include/
          )


# Finally the library itself
find_library(Yosys_LIBRARY
             NAMES yosys-lib
             PATHS ${yosys_PKGCONF_LIBRARY_DIRS}
             HINTS ${Readline_ROOT_DIR}/lib
             )

set(Yosys_LIBRARIES ${Yosys_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(../yosys DEFAULT_MSG Yosys_LIBRARIES Yosys_INCLUDE_DIR)

mark_as_advanced(
        Yosys_INCLUDE_DIR
        Yosys_core_LIBRARY
)