set( NAME trackerAruco )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwRuntime
    fwCore
    fwData
    fwDataTools
    fwServices
    fwTools
    fwMath
    fwCom
    arData
    fwMedData
    arServices
    cvIO

)
set( REQUIREMENTS fwData
                  servicesReg
                  arData
)
set( CONAN_DEPS
    SIGHT_CONAN_OPENCV
)
