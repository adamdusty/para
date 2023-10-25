if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/para-${PROJECT_VERSION}"
      CACHE PATH ""
  )
endif()

set(package para)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT para_Development
)

install(
    TARGETS para_para
    EXPORT paraTargets
    RUNTIME COMPONENT para_Runtime
    LIBRARY COMPONENT para_Runtime
    NAMELINK_COMPONENT para_Development
    ARCHIVE COMPONENT para_Development
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    para_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(para_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${para_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT para_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${para_INSTALL_CMAKEDIR}"
    COMPONENT para_Development
)

install(
    EXPORT paraTargets
    NAMESPACE para::
    DESTINATION "${para_INSTALL_CMAKEDIR}"
    COMPONENT para_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
