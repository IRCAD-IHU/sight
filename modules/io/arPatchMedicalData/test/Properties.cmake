
set( NAME arPatchMedicalDataTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwTest
    core
    fwData
    fwServices
    fwRuntime
    fwMedData
    arMDSemanticPatch
)
set( REQUIREMENTS fwData servicesReg ioAtoms patchMedicalData arPatchMedicalData)
