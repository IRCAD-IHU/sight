set( NAME OgreViewer )
set( VERSION 0.5 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    visuOgre                # Start the module, allow to use fwRenderOgre
    module_ui_qt                   # Start the module, allow dark theme
    patchMedicalData        # Patch old atoms data

    visuOgreQt              # Enable Ogre to render things in Qt window.

    # Objects declaration
    module_services
    data
    ioPacs

    # UI declaration/Actions
    module_ui_base
    style
    uiVisuOgre
    uiIO
    uiImageQt
    uiTF
    flatIcon
    uiMeasurementQt

    # Reader
    ioVTK
    ioAtoms
    ioGdcm

    # Services
    scene2D
    ctrlCamp
    ctrlTF
    uiMeasurement
    opImageFilter
    ioPacs
    uiMedDataQt
    ctrlSelection

    # Configuration launchers
    ogreConfig

    # Generic Scene
    visuOgreAdaptor
    )

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        OgreViewer_AppCfg
) # Main application's configuration to launch
