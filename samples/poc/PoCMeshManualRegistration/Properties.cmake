
set( NAME PoCMeshManualRegistration)
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    ioITK
    fwlauncher
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    ioVTK
    ioData
    uiImageQt
    uiVisuQt
    ctrlSelection
    uiReconstructionQt
    uiMedDataQt
    appXml
    opVTKMesh
    media
    dataManagerConfig
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES PoCMeshManualRegistration)
