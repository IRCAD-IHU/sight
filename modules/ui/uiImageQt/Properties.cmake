
set( NAME uiImageQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwGui
    fwGuiQt
    fwMath
    fwRuntime
    fwServices
    fwTools
    fwDataTools
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    ${CONAN_QT}
)
set( WARNINGS_AS_ERRORS ON)
