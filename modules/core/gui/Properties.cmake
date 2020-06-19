
set( NAME gui )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwDataTools
    fwGui
    fwRuntime
    fwServices
    fwTools
    fwThread
    fwJobs
    fwMedData
    fwDataCamp
    fwActivities
)
set( REQUIREMENTS
    dataReg
    servicesReg
)

set( CONAN_DEPS
    ${CONAN_BOOST}
)