sight_add_target( Tuto04MeshGeneratorCpp TYPE APP UNIQUE ON )

add_dependencies(Tuto04MeshGeneratorCpp 
                 sightrun
                 data
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_service
                 module_io_matrix
                 module_io_vtk
)

target_link_libraries(Tuto04MeshGeneratorCpp PUBLIC 
                      data
                      geometry_data
                      service
                      ui_base
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
        Tuto04MeshGeneratorCpp_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto04MeshGeneratorCpp)
