set( NAME ioPacs )
set( VERSION 0.1 )
set( TYPE MODULE )
set( PRIORITY 1 )
set( DEPENDENCIES
    fwCore
    fwThread
    fwCom
    fwGui
    fwGuiQt
    fwPacsIO
    fwRuntime
    fwTools
    fwData
    fwDataTools
    fwMedData
    fwMedDataTools
    fwServices
    fwIO
    fwPreferences
)
set( REQUIREMENTS )
set( CONAN_DEPS
    SIGHT_CONAN_DCMTK
    SIGHT_CONAN_QT
)
