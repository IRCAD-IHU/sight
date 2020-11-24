set( NAME calibration3d )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    eigenTools
)
set( REQUIREMENTS  )
set( DISABLE_PCH TRUE)
set( CONAN_DEPS
    SIGHT_CONAN_CERES-SOLVER
    SIGHT_CONAN_OPENCV
    SIGHT_CONAN_FFMPEG
    SIGHT_CONAN_QT
)
