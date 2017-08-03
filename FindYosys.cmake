find_package(PkgConfig)
pkg_check_modules(PC_LIBCGRAPH yosys)

# Include dir
find_path(yosys_INCLUDE_DIR
          kernel/yosys.h
          PATHS ${yosys_PKGCONF_INCLUDE_DIRS}
          )


# Finally the library itself
find_library(yosys_LIBRARY
             NAMES yosys-lib
             PATHS ${yosys_PKGCONF_LIBRARY_DIRS}
             )

set(yosys_LIBRARIES ${yosys_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yosys DEFAULT_MSG yosys_LIBRARIES yosys_INCLUDE_DIR)

mark_as_advanced(
        yosys_INCLUDE_DIR
        yosys_core_LIBRARY
)