
set( NAME uiImageQml )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwRuntime
    fwTools
    fwDataTools
    fwQml
)
set( REQUIREMENTS styleQml )
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
set( WARNINGS_AS_ERRORS ON)
