sight_add_target( module_viz_sample TYPE MODULE WARNINGS_AS_ERRORS ON )


add_dependencies(module_viz_sample 
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_ui_qt
)

target_link_libraries(module_viz_sample PUBLIC ui_qt)
