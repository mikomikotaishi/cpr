function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Command failed: ${ARGV}\n${output}\n${error}")
    endif()
endfunction()

set(prefix "${CPR_CONSUMER_BINARY_DIR}/install")
set(config_args)
set(ctest_config_args)
if(CPR_TEST_CONFIG)
    list(APPEND config_args --config "${CPR_TEST_CONFIG}")
    list(APPEND ctest_config_args -C "${CPR_TEST_CONFIG}")
endif()
run("${CMAKE_COMMAND}" --install "${CPR_BUILD_DIR}/cpr" --prefix "${prefix}"
    ${config_args})
run("${CMAKE_COMMAND}" --install "${CPR_BUILD_DIR}/include" --prefix "${prefix}"
    ${config_args})
# Configure the consumer the same way as the parent build, so cprConfig.cmake's
# find_dependency() calls (e.g. CURL from vcpkg) succeed. Settings go into an
# initial cache file because run() would split list values on ';'.
set(consumer_args)
if(CPR_TEST_GENERATOR)
    list(APPEND consumer_args -G "${CPR_TEST_GENERATOR}")
endif()
set(consumer_prefix_path "${prefix}")
if(CPR_TEST_PREFIX_PATH)
    string(REPLACE "|" ";" parent_prefix_path "${CPR_TEST_PREFIX_PATH}")
    list(APPEND consumer_prefix_path ${parent_prefix_path})
endif()
# Start clean: a stale build dir made with another generator cannot be reused.
file(REMOVE_RECURSE "${CPR_CONSUMER_BINARY_DIR}/build")
set(consumer_cache "${CPR_CONSUMER_BINARY_DIR}/initial-cache.cmake")
file(WRITE "${consumer_cache}" "set(CMAKE_PREFIX_PATH [==[${consumer_prefix_path}]==] CACHE STRING \"\")\n")
if(CPR_TEST_TOOLCHAIN_FILE)
    file(APPEND "${consumer_cache}" "set(CMAKE_TOOLCHAIN_FILE [==[${CPR_TEST_TOOLCHAIN_FILE}]==] CACHE FILEPATH \"\")\n")
endif()
if(CPR_TEST_VCPKG_TARGET_TRIPLET)
    file(APPEND "${consumer_cache}" "set(VCPKG_TARGET_TRIPLET [==[${CPR_TEST_VCPKG_TARGET_TRIPLET}]==] CACHE STRING \"\")\n")
endif()
if(CPR_TEST_CONFIG)
    file(APPEND "${consumer_cache}" "set(CMAKE_BUILD_TYPE [==[${CPR_TEST_CONFIG}]==] CACHE STRING \"\")\n")
endif()

run("${CMAKE_COMMAND}" -S "${CPR_CONSUMER_SOURCE_DIR}"
    -B "${CPR_CONSUMER_BINARY_DIR}/build" ${consumer_args}
    -C "${consumer_cache}"
    "-DCPR_TEST_CUSTOM_CONTEXT=${CPR_TEST_CUSTOM_CONTEXT}"
    "-DBOOST_ROOT=${CPR_TEST_BOOST_ROOT}"
    "-DBoost_ROOT=${CPR_TEST_BOOST_ROOT}"
    "-DCMAKE_DISABLE_FIND_PACKAGE_Boost=${CPR_TEST_DISABLE_BOOST}")
run("${CMAKE_COMMAND}" --build "${CPR_CONSUMER_BINARY_DIR}/build"
    ${config_args})
run("${CMAKE_CTEST_COMMAND}" --test-dir "${CPR_CONSUMER_BINARY_DIR}/build"
    ${ctest_config_args} --output-on-failure)
