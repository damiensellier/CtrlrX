find_package(Git)

if(NOT GIT_FOUND)
    message(WARNING "Git executable was not found. Git submodule checks will not be executed. "
        "If you have any build issues at all, start by adding git executable to the PATH and "
        "rerun cmake to not see this warning again.")

    function(git_submodule_check root_path)
        # no-op; ARGN swallows EXCLUDE and friends so callers need no #ifdef.
    endfunction()
else()

    # Turn the raw output of a command into a cmake list, one entry per line.
    function(lines2list text_var)
        string(REGEX REPLACE "[\r\n]+$" "" text "${${text_var}}")  # drop the trailing newline(s)
        string(REPLACE ";" "\\;" text "${text}")            # protect literal semicolons
        string(REGEX REPLACE "\r?\n" ";" text "${text}")    # one list entry per line
        set(${text_var} "${text}" PARENT_SCOPE)
    endfunction()

    # include(git_submodules)
    # git_submodule_check(<root_path> [EXCLUDE <regex>...])
    #
    # Initialise any missing submodule of the repository at <root_path>, and warn
    # about any that are out of date.
    #
    # EXCLUDE takes CMake regular expressions, matched against the submodule path
    # exactly as git reports it: repository-relative, forward slashes, no trailing
    # slash (eg "repo_x_dir/submodule_dir").  A submodule matching any of the expressions is
    # left completely alone. That is, it is not initialised, not warned about, and not registered
    # as a configure dependency.  Use it for submodules that are not build inputs:
    #
    #     git_submodule_check(${CMAKE_CURRENT_SOURCE_DIR} EXCLUDE "^repo_x_dir/")
    #
    # Anchor the expressions; MATCHES searches anywhere in the string, so a bare
    # "Panels" would also exclude a hypothetical "source/repo_x_dir".
    function(git_submodule_check root_path)

        cmake_parse_arguments(PARSE_ARGV 1 GSC "" "" "EXCLUDE")

        if(GSC_UNPARSED_ARGUMENTS)
            message(FATAL_ERROR "git_submodule_check: unexpected argument(s): ${GSC_UNPARSED_ARGUMENTS}")
        endif()
        if(GSC_KEYWORDS_MISSING_VALUES)
            message(FATAL_ERROR "git_submodule_check: ${GSC_KEYWORDS_MISSING_VALUES} given without a value")
        endif()

        # Sources may have been downloaded as a zip/tarball rather than cloned;
        # there are no submodules to check in that case, so silently do nothing.
        # (.git is a directory in a normal clone, a file in a worktree/submodule.)
        if(NOT EXISTS "${root_path}/.git")
            message(WARNING "This does not appear to be a git repository. Skipping submodule checks.")
            return()
        endif()

        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule status
            WORKING_DIRECTORY ${root_path}
            OUTPUT_VARIABLE submodule_status
            ERROR_VARIABLE git_error
            RESULT_VARIABLE git_result
            )

        if(git_result)
            message(WARNING "Git submodule status failed, skipping submodule checks: ${git_error}")
            return()
        endif()

        # git submodule status output not guaranteed to be stable,
        # may need to check GIT_VERSION_STRING and do some fiddling in the
        # future...

        lines2list(submodule_status)

        foreach(line ${submodule_status})
            string(REGEX MATCH "(.)[0-9a-f]+ ([^\( ]+) ?" _ignored "${line}")
            set(status "${CMAKE_MATCH_1}")
            set(submodule_path "${CMAKE_MATCH_2}")

            # Excluded submodules are skipped whole: no init, no warning, and no
            # CMAKE_CONFIGURE_DEPENDS entry (which would re-run cmake, and so
            # re-run this check, whenever they are added or removed by hand).
            set(excluded FALSE)
            foreach(pattern IN LISTS GSC_EXCLUDE)
                if("${submodule_path}" MATCHES "${pattern}")
                    set(excluded TRUE)
                    break()
                endif()
            endforeach()
            if(excluded)
                message(STATUS "Ignoring excluded submodule ${submodule_path}")
                continue()
            endif()

            if("${status}" STREQUAL "-")  # missing submodule
                message(STATUS "Initialising new submodule ${submodule_path}...")
                execute_process(
                    COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive ${submodule_path}
                    WORKING_DIRECTORY ${root_path}
                    RESULT_VARIABLE git_result
                    )
                if(git_result)
                    message(FATAL_ERROR "Git submodule init failed for ${submodule_path}")
                endif()

            elseif(NOT "${status}" STREQUAL " ")
                message(WARNING "Git submodule ${submodule_path} is out of date. Run the following command to fix: "
                    "git submodule update --init --recursive")
            endif()

            # Force a re-run of cmake if the submodule's .git file changes or is changed (ie accidental deinit)
            get_filename_component(submodule_abs_path ${submodule_path} ABSOLUTE BASE_DIR ${root_path})
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${submodule_abs_path}/.git)
            # same if the HEAD file in the submodule's directory changes (ie commit changes).
            # This will at least print the 'out of date' warning
            set(submodule_head "${root_path}/.git/modules/${submodule_path}/HEAD")
            if(EXISTS "${submodule_head}")
                set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${submodule_head})
            endif()

        endforeach()
    endfunction()

endif()
