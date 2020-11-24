
set( NAME calibrationActivity )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwCore
    fwData
    fwActivities
    fwRuntime
)
set( REQUIREMENTS
    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    material                # Start the module, load Ogre's materials
    visuOgreQt              # Enable Ogre to render things in Qt window.
    validators
    activities

    # Objects declaration
    servicesReg             # fwService
    fwData
    arData

    # UI declaration/Actions
    gui
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
    videoTools
    registrationCV
    ctrlCamp

    # Generic Scene
    visuOgreAdaptor
)
