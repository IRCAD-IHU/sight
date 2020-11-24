
set( NAME ioDicom )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwDicomIOFilter
    fwDicomIOFilterQt
    fwGui
    fwGuiQt
    fwJobs
    fwLog
    fwRuntime
    fwServices
    fwTools
    fwIO
    fwData
    fwDataTools
    fwMedData
    fwMedDataTools
    fwThread
    )
set( REQUIREMENTS
    fwData
    servicesReg
    )
set( CONAN_DEPS
    SIGHT_CONAN_QT
)