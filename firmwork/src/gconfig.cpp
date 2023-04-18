#include "resource.h"

int global_menu_choose = 0;

struct MenuLine _rocker_function_menu = {
    (char*)"RK-F-pick ", MenuConfig::R_MOUSE, MenuConfig::R_MOUSE,
    MenuConfig::_R_MAX - 1};
struct MenuLine _rocker_mouse_scale_menu = {(char*)"RK-M-Scale", 20, 0, 99};
struct MenuLine _rocker_wheel_scale_menu = {(char*)"RK-W-Scale", 30, 0, 99};

struct MenuLine _encode_function_menu = {
    (char*)"EC-F-pick", MenuConfig::E_VOLUME, MenuConfig::E_VOLUME,
    MenuConfig::_E_MAX - 1};
struct MenuLine _encode_volume_scale_menu = {(char*)"EC-V-Scale", 2, 0, 99};

MenuConfig global_menu_config = {
    0,
    {&_rocker_function_menu, &_rocker_mouse_scale_menu,
     &_rocker_wheel_scale_menu, &_encode_function_menu,
     &_encode_volume_scale_menu}};

FunctionChoose global_function_choose = {0, 0};
// FunctionChoose global_function_choose = {0, 0, FunctionChoose::F_MOUSE};