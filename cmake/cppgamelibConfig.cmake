include("${CMAKE_CURRENT_LIST_DIR}/cppgamelib.cmake")

set_target_properties(cppgamelib::cppgamelib PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO "RELEASE"
        MAP_IMPORTED_CONFIG_MINSIZEREL "RELEASE" )