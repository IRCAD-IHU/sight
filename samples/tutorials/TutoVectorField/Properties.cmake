
set( NAME TutoVectorField )
set( VERSION 0.1 )
set( TYPE APP)
set( UNIQUE TRUE )
set( DEPENDENCIES)
set( REQUIREMENTS
    dataReg
    ctrlSelection
    gui
    guiQt
    ioData
    ioVTK
    servicesReg
    uiVisuQt
    uiImageQt
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES TutoVectorField)
