SET (SOURCES vtkGraphCutTest.cxx ) # vtkPrioQueueTest.cxx vtkColorValueTest.cxx )

FOREACH (source IN LISTS SOURCES)
    GET_FILENAME_COMPONENT(class ${source} NAME_WE)
    ADD_EXECUTABLE(${class} ${source})
    TARGET_LINK_LIBRARIES(${class} vtkGraphCut)
    ADD_TEST(${class} ${EXECUTABLE_OUTPUT_PATH}/${class})
ENDFOREACH()

#ADD_TEST(vtkGraphCutTest ${EXECUTABLE_OUTPUT_PATH}/vtkGraphCutTest)

vtk_add_test_cxx(NO_VALID NO_OUTPUT
  #    TestFlashReader.cxx  there is not data for this test
  TestEnzoReader.cxx
  )
vtk_test_cxx_executable(${vtk-module}CxxTests)
