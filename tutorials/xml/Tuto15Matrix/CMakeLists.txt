sight_add_target( Tuto15Matrix TYPE APP )



add_dependencies(Tuto15Matrix 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_base
                 module_io_matrix
                 module_io_vtk
                 module_io_atoms
                 module_geometry_base
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto15Matrix_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto15Matrix)
