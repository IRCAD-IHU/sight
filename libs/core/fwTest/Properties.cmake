
set( NAME fwTest )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataCamp
    fwMedData
    fwTools
)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    ${CONAN_CPPUNIT}
)
set (WARNINGS_AS_ERRORS ON)
