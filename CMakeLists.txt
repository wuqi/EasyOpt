SET(MAHI_GUI_TGZ ${CMAKE_CURRENT_SOURCE_DIR}/mahi-gui.tar.gz)
include(FetchContent)
FetchContent_Declare(mahi-gui URL ${MAHI_GUI_TGZ})
FetchContent_MakeAvailable(mahi-gui)

add_executable(cli_gui_test "main.cpp")
target_link_libraries(cli_gui_test mahi::gui)