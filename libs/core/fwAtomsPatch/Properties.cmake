
set( NAME fwAtomsPatch )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwAtomConversion fwAtoms fwCore fwTools fwRuntime )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET fwAtomConversion )
set( CONAN_DEPS
    SIGHT_CONAN_BOOST
    SIGHT_CONAN_CAMP
)