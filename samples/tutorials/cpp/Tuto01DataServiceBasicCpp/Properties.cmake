set( NAME Tuto01DataServiceBasicCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( START ON )
set( DEPENDENCIES
    data
)
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_ui_qt                   # Start the module, load qt implementation of module_ui_base

    # Objects declaration
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base

    # Reader
    ioVTK           # contains the reader and writer for VTK files (image and mesh).

    # Services
    visuBasic   # loads basic rendering services for images and meshes.

)
set( WARNINGS_AS_ERRORS ON )
