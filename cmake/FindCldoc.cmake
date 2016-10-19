# Module to find a cldoc installation
#
# This modules finds if cldoc is installed, e.g. you can do
# find_package(Cldoc REQUIRED)
#
# it defines the following variable
# CLDOC_FOUND -  true if cldoc is found
# CLDOC_EXECUTABLE - path to the executable
#
# it wil also define the following function :
#
#
# add_cldoc_documentation(FILES file1 file2
#                         [OUTPUT directory]
#                         [LANGUAGE (c++|c|objc)]
#                         [BASEDIR  dir]
#                         [MERGE_MD file1.md file2.md]
#                         [REPORT]
#                         [STATIC_HTML]
#                         [CUSTOM_JS file1 file2]
#                         [CUSTOM_CSS file1 file2])
#
# Adds a cldoc documentation. It will generate the target doc (excluded from all) that will generate the website and the target serve-doc to serve the documentation.
# - FILES : the mandatory list of file to parse
# - OUTPUT : the output directory
# - LANGUAGE : specifies the language
# - BASEDIR : the basedir
# - MERGE_MD : list of markdown file to merge with documentation
# - REPORT: add static report. Will be set by default in Debug mode
# - STATIC_HTML : serve a static html site, no js
# - CUSTOM_JS : adds custom js
# - CUSTOMS_CSS : adds custom css


if(CLDOC_EXECUTABLE)
	set(Cldoc_FIND_QUIETLY TRUE)
endif(CLDOC_EXECUTABLE)

find_program(CLDOC_EXECUTABLE cldoc)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cldoc
								DEFAULT_MSG
								CLDOC_EXECUTABLE)

mark_as_advanced(CLDOC_EXECUTABLE)


if(CLDOC_FOUND)

	include(CMakeParseArguments)

	function(add_cldoc_documentation)
		set(options REPORT STATIC_HTML)
		set(oneValArgs OUTPUT LANGUAGE BASEDIR)
		set(multiValArgs FILES FLAGS MERGE_MD CUSTOM_JS CUSTOM_CSS)

		cmake_parse_arguments(ARG "${options}" "${oneValArgs}" "${multiValArgs}" ${ARGN})


		if(NOT ARG_FILES)
			message(FATAL_ERROR "Missing mandatory arguments FILES to add_cldoc_documentation")
		endif(NOT ARG_FILES)

		if(NOT ARG_OUTPUT)
			message(STATUS "Setting cldoc output to ${CMAKE_CURRENT_BINARY_DIR}")
			set(ARG_OUTPUT ${CMAKE_CURRENT_BINARY_DIR})
		endif(NOT ARG_OUTPUT)

		#constructs the list of arguments
		set(CLDOC_GEN_ARGS "generate")
		list(APPEND CLDOC_GEN_ARGS ${ARG_FLAGS} "--")

		if(ARG_REPORT OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
			list(APPEND CLDOC_GEN_ARGS "--report")
		endif(ARG_REPORT OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

		if(ARG_LANGUAGE)
			list(APPEND CLDOC_GEN_ARGS "--language" ${ARG_LANGUAGE})
		endif(ARG_LANGUAGE)

		if(ARG_BASEDIR)
			list(APPEND CLDOC_GEN_ARGS "--basedir" ${ARG_BASEDIR})
		endif(ARG_BASEDIR)

		if(ARG_MERGE_MD)
			foreach(m ${ARG_MERGE_MD})
				list(APPEND CLDOC_GEN_ARGS "--merge" ${m})
			endforeach(m ${ARG_MERGE_MD})
		endif(ARG_MERGE_MD)

		if(ARG_STATIC_HTML)
			list(APPEND CLDOC_GEN_ARGS "--static")
		endif(ARG_STATIC_HTML)

		if(ARG_CUSTOM_JS)
			list(APPEND CLDOC_GEN_ARGS "--custom-js" ${ARG_CUSTOM_JS})
		endif(ARG_CUSTOM_JS)

		if(ARG_CUSTOM_CSS)
			list(APPEND CLDOC_GEN_ARGS "--custom-css" ${ARG_CUSTOM_CSS})
		endif(ARG_CUSTOM_CSS)

		list(APPEND CLDOC_GEN_ARGS "--output" ${ARG_OUTPUT})

		list(APPEND CLDOC_GEN_ARGS ${ARG_FILES})

		add_custom_target(doc ${CLDOC_EXECUTABLE} ${CLDOC_GEN_ARGS}
						COMMENT "Generating documentation in ${ARG_OUTPUT}"
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

		add_custom_target(serve-doc ${CLDOC_EXECUTABLE} serve ${CLDOC_REL_OUTPUT}
						COMMENT "Serving documentation from ${ARG_OUTPUT}. Press Ctrl-c to end"
						WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
						DEPENDS doc)

	endfunction(add_cldoc_documentation)

endif(CLDOC_FOUND)
