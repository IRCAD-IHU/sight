
set( NAME ARCalibration )
set( VERSION 0.3 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    extDataReg
    arDataReg
    servicesReg

    patchMedicalData
    arPatchMedicalData

    gui
    guiQt

    io
    ioAtoms
    ioCalibration
    ioARActivity

    uiIO
    uiGenericQt
    uiMedDataQt

    media
    mediaExt
    arMedia

    activities
    calibrationActivity

    appXml2
    fwlauncher
    memory

    preferences
    uiPreferences
    )

bundleParam(appXml2 PARAM_LIST config parameters PARAM_VALUES ARCalibrationBase ARCalibrationAppBase)
