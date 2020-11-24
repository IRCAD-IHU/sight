
set( NAME uiVisuOgre )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        fwCore
        fwData
        fwCom
        fwRuntime
        fwServices
        fwTools
        fwGui
        fwGuiQt
        fwRenderOgre
        fwIO
)

set( REQUIREMENTS
        material
        uiIO
)

set( CONAN_DEPS
    SIGHT_CONAN_QT
)