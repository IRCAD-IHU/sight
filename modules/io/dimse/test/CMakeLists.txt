sight_add_target( module_io_dimseTest TYPE TEST )


add_dependencies(module_io_dimseTest 
                 data
                 module_service
                 module_ui_base
                 module_io_dimse
)

target_link_libraries(module_io_dimseTest PUBLIC 
                      core
                      service
                      io_dimse
                      utestData
)
