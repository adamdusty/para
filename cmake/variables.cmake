# ---- Warning guard ----

# target_include_directories with the SYSTEM modifier will request the compiler
# to omit warnings from the provided paths, if the compiler supports that
# This is to provide a user experience similar to find_package when
# add_subdirectory or FetchContent is used to consume this project
set(warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      para_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for bare's includes, disabling warnings"
      ON
  )
  mark_as_advanced(para_INCLUDES_WITH_SYSTEM)
  if(para_INCLUDES_WITH_SYSTEM)
    set(warning_guard SYSTEM)
  endif()
endif()
