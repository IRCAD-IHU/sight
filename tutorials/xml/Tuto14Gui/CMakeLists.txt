sight_add_target( Tuto14Gui TYPE APP )



add_dependencies(Tuto14Gui 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
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
        Tuto14Gui_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto14Gui)
