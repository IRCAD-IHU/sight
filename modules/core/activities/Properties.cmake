
set( NAME module_activities )
set( VERSION 0.1 )
set( TYPE MODULE )

set( DEPENDENCIES
    core
    data
    fwGui
    services
)
set( REQUIREMENTS
    data
    module_services
)

if(NOT ${ENABLE_QML_APPLICATION})

    list(APPEND DEPENDENCIES
        fwGuiQt
        )
    list(APPEND REQUIREMENTS
        guiQt
        )
    add_definitions ( -DKEEP_OLD_SERVICE )
endif()
