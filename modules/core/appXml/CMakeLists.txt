sight_add_target( module_appXml TYPE MODULE START ON )


add_dependencies(module_appXml 
                 module_service
)

target_link_libraries(module_appXml PUBLIC 
                      core
                      service
)
