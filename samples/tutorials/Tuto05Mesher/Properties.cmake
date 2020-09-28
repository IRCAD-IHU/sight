
set( NAME Tuto05Mesher )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
    ioVTK
    visuVTKQt
    uiIO
    vtkSimpleNegato
    vtkSimpleMesh
    opVTKMesh # provides services to generate a mesh from an image.
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES MesherConfig)
