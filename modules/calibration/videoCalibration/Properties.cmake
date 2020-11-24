set( NAME videoCalibration )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    arData
    arServices
    calibration3d
    fwCom
    fwCore
    fwData
    fwDataTools
    fwGui
    fwRuntime
    fwServices
    fwTools
    fwPreferences
    cvIO
)
set( REQUIREMENTS fwData servicesReg)
set( CONAN_DEPS
    SIGHT_CONAN_OPENCV
)