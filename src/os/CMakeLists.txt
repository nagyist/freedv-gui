if(APPLE)
    set(OS_WRAP_FILES osx_interface.mm)
elseif(WIN32)
    set(OS_WRAP_FILES windows_interface.cpp)
else()
    set(OS_WRAP_FILES osx_stubs.cpp)
endif()

add_library(fdv_os_wrapper STATIC
    ${OS_WRAP_FILES}
)

if(BOOTSTRAP_WXWIDGETS)
    add_dependencies(fdv_os_wrapper wx::core wx::base wx::aui wx::html wx::net wx::adv wx::propgrid wx::xrc)
    target_compile_definitions(fdv_os_wrapper PRIVATE ${WXBUILD_BUILD_DEFS})
    target_include_directories(fdv_os_wrapper PRIVATE ${WXBUILD_INCLUDES})
endif(BOOTSTRAP_WXWIDGETS)

target_include_directories(fdv_os_wrapper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/..)