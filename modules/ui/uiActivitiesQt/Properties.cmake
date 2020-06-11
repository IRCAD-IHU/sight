
set( NAME uiActivitiesQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCore
    fwActivities
    fwCom
    fwData
    fwDataCamp
    fwGui
    fwGuiQt
    fwMedData
    fwRuntime
    fwServices
    fwTools
    fwMedDataTools
)
set( REQUIREMENTS
    guiQt
    dataReg
    servicesReg
)

set( CONAN_DEPS
    ${CONAN_QT}
)

set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
