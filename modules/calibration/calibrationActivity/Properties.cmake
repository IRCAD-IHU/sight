
set( NAME calibrationActivity )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    data
)
set( REQUIREMENTS
    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    visuOgreQt              # Enable Ogre to render things in Qt window.
    validators
    activities

    # Objects declaration
    module_services             # fwService
    data

    # UI declaration/Actions
    module_gui
    media
    uiTools
    uiIO
    uiCalibration
    uiPreferences
    uiMedDataQt

    # Reader
    ioData
    ioCalibration
    videoQt

    # Services
    videoCalibration
    videoCharucoCalibration
    videoOpenCV
    videoOpenni
    videoTools
    registrationCV
    ctrlCamp

    # Generic Scene
    visuOgreAdaptor
)
