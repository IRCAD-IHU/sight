
set( NAME Tuto08GenericScene )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
    ioData # contains reader/writer for mesh (.trian) or matrix (.trf)
    ioVTK
    uiIO
    uiVisuQt # contains several editors for visualization
    uiImageQt # contains several editors on image
    visuVTKQt
    visuVTKAdaptor # contains adaptors for the generic scene
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES Tuto08GenericScene)
