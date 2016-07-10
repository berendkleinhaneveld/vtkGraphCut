IF(BUILD_TEST)
    # Find all cxx files in the test folder
    FILE(GLOB SOURCES
        "*.cxx"
    )

    # Add test targets for each of the tests found
    FOREACH(source IN LISTS SOURCES)
        GET_FILENAME_COMPONENT(class ${source} NAME_WE)
        ADD_EXECUTABLE(${class} ${source})
        TARGET_LINK_LIBRARIES(${class} vtkGraphCut)
        INCLUDE_DIRECTORIES("../..")
        ADD_TEST(${class} ${CXX_TEST_PATH}/${class})
    ENDFOREACH()
ENDIF(BUILD_TEST)
