set( NAME Tuto06GuiQml ) # Name of the application
set( VERSION 0.2 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( START ON)
set( DEPENDENCIES
    data
    fwServices
    fwQml
)
set( REQUIREMENTS # The modules used by this application
    data
    servicesReg
    gui
    guiQt
    )
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
