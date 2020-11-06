set( NAME ExDicomSegmentation )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    fwMedData
    servicesReg
    gui
    guiQt
    ctrlSelection
    ioData
    ioGdcm
    ioVTK
    uiIO
    uiVisuQt
    uiReconstructionQt
    uiMedDataQt
    uiActivitiesQt
    uiImageQt
    visuVTKAdaptor
    visuVTKQt
    opVTKMesh
    opImageFilter
    fwlauncher
    appXml
    activities
    media
    ioActivity
    2DVisualizationActivity
    3DVisualizationActivity
    preferences
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExDicomSegmentationAppConfigBase)
