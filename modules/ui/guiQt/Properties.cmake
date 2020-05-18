
set( NAME guiQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    fwActivities
    fwCom
    fwCore
    fwData
    fwDataCamp
    fwDataTools
    fwMedData
    fwGui
    fwQt
    fwGuiQt
    fwRuntime
    fwServices
    fwTools
)
set( REQUIREMENTS
    gui
)
set( PLUGINS
    platforms
    imageformats
    styles
)
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_QT}
)
