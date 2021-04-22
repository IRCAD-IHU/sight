sight_add_target( io_vtk TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS ON )

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(io_vtk PUBLIC
        VTK::FiltersGeometry
        VTK::FiltersModeling
        VTK::IOExport
        VTK::IOGeometry
        VTK::IOImage
        VTK::IOLegacy
        VTK::IOPLY
        VTK::IOXML
        VTK::RenderingFreeType
        VTK::RenderingOpenGL2 
    )

else()
# Necessary for all related unit-tests and utilities
    find_package(GLEW)
    # VTK7
    target_include_directories(io_vtk SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(io_vtk PUBLIC  
        vtkFiltersGeometry
        vtkFiltersModeling
        vtkIOExport
        vtkIOGeometry
        vtkIOImage
        vtkIOLegacy
        vtkIOPLY
        vtkIOXML
        vtkRenderingFreeType
        vtkRenderingOpenGL2
        ${GLEW_LIBRARIES}
    )

endif()

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(io_vtk PRIVATE ${VTK_DEFINITIONS})

target_link_libraries(io_vtk PUBLIC core data io_base geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)