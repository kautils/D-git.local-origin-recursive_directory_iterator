if(NOT DEFINED ${KAUTIL_THIRD_PARTY_DIR})
    set(KAUTIL_THIRD_PARTY_DIR ${CMAKE_BINARY_DIR})
    file(MAKE_DIRECTORY "${KAUTIL_THIRD_PARTY_DIR}")
endif()

macro(git_clone url)
    get_filename_component(file_name ${url} NAME)
    if(NOT EXISTS ${KAUTIL_THIRD_PARTY_DIR}/kautil_cmake/${file_name})
        file(DOWNLOAD ${url} "${KAUTIL_THIRD_PARTY_DIR}/kautil_cmake/${file_name}")
    endif()
    include("${KAUTIL_THIRD_PARTY_DIR}/kautil_cmake/${file_name}")
    unset(file_name)
endmacro()


if(NOT DEFINED KAUTIL_BUILD_SHARED)
    set(KAUTIL_BUILD_SHARED TRUE CACHE STRING "build shared library which is possible to extern via dlopen / LoadLibrary" FORCE)
endif()


git_clone(https://raw.githubusercontent.com/kautils/CMakeLibrarytemplate/v0.0.1/CMakeLibrarytemplate.cmake)
git_clone(https://raw.githubusercontent.com/kautils/CMakeFetchKautilModule/v0.0.1/CMakeFetchKautilModule.cmake)

CMakeFetchKautilModule(c11_string_allocator GIT https://github.com/kautils/c11_string_allocator.git REMOTE origin TAG v0.0.1)
find_package(KautilC11StringAllocator.0.0.1.static REQUIRED)

set(walk_cmake_unsetter __libs)
if(MINGW EQUAL 1)
    # ref https://learn.microsoft.com/en-us/windows/win32/api/shlwapi/
    list(APPEND __libs shlwapi )
endif()
list(APPEND __libs kautil::c11_string_allocator::0.0.1::static)


set(module_name recursive_directory_iterator)
get_filename_component(__include_dir "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
unset(srcs)
set(srcs ${CMAKE_CURRENT_LIST_DIR}/recursive_directory_iterator.cc)
set(${module_name}_common_pref
    #DEBUG_VERBOSE
    MODULE_PREFIX kautil wstd fs
    MODULE_NAME ${module_name}
    INCLUDES $<BUILD_INTERFACE:${__include_dir}> $<INSTALL_INTERFACE:include> 
    SOURCES ${srcs}
    LINK_LIBS ${__libs} 
    EXPORT_NAME_PREFIX ${PROJECT_NAME}
    EXPORT_VERSION ${PROJECT_VERSION}
    EXPORT_VERSION_COMPATIBILITY AnyNewerVersion
        
    DESTINATION_INCLUDE_DIR include/kautil/wstd/fs
    DESTINATION_CMAKE_DIR cmake
    DESTINATION_LIB_DIR lib
)

CMakeLibraryTemplate(${module_name} EXPORT_LIB_TYPE static ${${module_name}_common_pref} )
CMakeLibraryTemplate(${module_name} EXPORT_LIB_TYPE shared ${${module_name}_common_pref} )

set(__t ${${module_name}_static_tmain})
add_executable(${__t})
target_sources(${__t} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/unit_test.cc)
target_link_libraries(${__t} PRIVATE ${${module_name}_static})
target_compile_definitions(${__t} PRIVATE ${${module_name}_static_tmain_ppcs})



if(${KAUTIL_BUILD_SHARED})
    CMakeFetchKautilModule(sharedlib GIT https://github.com/kautils/sharedlib.git REMOTE origin TAG v0.0.1)
    find_package(KautilSharedlib.0.0.1.static REQUIRED)
    
    set(__t ${${module_name}_shared_tmain})
    add_executable(${__t})
    target_sources(${__t} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/unit_test.cc)
    target_link_libraries(${__t} PRIVATE  kautil::sharedlib::0.0.1::static )
    target_compile_options(${__t} PRIVATE  -std=c++23 -O2)
    target_compile_definitions(${__t} PRIVATE 
            ${${module_name}_shared_tmain_ppcs}
            PATH_TO_SHARED_LIB="$<TARGET_FILE:${${module_name}_shared}>"
            )
    add_dependencies(${__t} ${${module_name}_shared} )
    
    list(APPEND walk_cmake_unsetter __installed_shared_lib_name)
    set(__installed_sahred_lib_name ${CMAKE_SHARED_LIBRARY_PREFIX}${module_name}${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(__path_to_shared_lib_build_interface $<TARGET_FILE:${${module_name}_shared}>)
    
    set(module_name recursive_directory_iterator_extern)
    message(${module_name})
    unset(srcs)
    set(srcs ${CMAKE_CURRENT_LIST_DIR}/recursive_directory_iterator_ext.cc)
    set(${module_name}_common_pref
        #DEBUG_VERBOSE
        MODULE_PREFIX kautil wstd fs 
        MODULE_NAME ${module_name}
        INCLUDES $<BUILD_INTERFACE:${__include_dir}> $<INSTALL_INTERFACE:include> 
        SOURCES ${srcs} 
        LINK_LIBS  kautil::sharedlib::0.0.1::static
        EXPORT_NAME_PREFIX ${PROJECT_NAME}Extern
        EXPORT_VERSION ${PROJECT_VERSION}
        EXPORT_VERSION_COMPATIBILITY AnyNewerVersion

        DESTINATION_INCLUDE_DIR include/kautil/wstd/fs
        DESTINATION_CMAKE_DIR cmake
        DESTINATION_LIB_DIR lib
    )

    CMakeLibraryTemplate(${module_name} EXPORT_LIB_TYPE static ${${module_name}_common_pref} )
    target_compile_definitions(${${module_name}_static} PRIVATE KAUTIL_RECURSIVE_DIRECTORY_ITERATOR_SO="${__path_to_shared_lib_build_interface}")
    target_compile_definitions(${${module_name}_static} INTERFACE 
            KAUTIL_RECURSIVE_DIRECTORY_ITERATOR_SO=$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/lib/${__installed_sahred_lib_name}>
            )
#    target_compile_definitions(${${module_name}_static} INTERFACE 
#            KAUTIL_RECURSIVE_DIRECTORY_ITERATOR_SO="$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${__installed_sahred_lib_name}">
#            )
    
    set(__t ${${module_name}_static_tmain})
    add_executable(${__t})
    target_sources(${__t} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/unit_test.cc)
    target_link_libraries(${__t} PRIVATE ${${module_name}_static})
    target_compile_definitions(${__t} PRIVATE ${${module_name}_static_tmain_ppcs})
    
    
endif()




foreach(__var ${walk_cmake_unsetter})
    unset(${__var})
endforeach()








