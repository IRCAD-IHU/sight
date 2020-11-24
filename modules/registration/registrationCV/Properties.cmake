set( NAME registrationCV )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    arData
    calibration3d
    fwCom
    fwCore
    fwData
    fwMath
    fwRuntime
    fwServices
    cvIO
)
set( REQUIREMENTS fwData
                  servicesReg
)
set( CONAN_DEPS
    SIGHT_CONAN_OPENCV
)