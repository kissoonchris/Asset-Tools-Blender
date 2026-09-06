cmake_minimum_required(VERSION 3.20)
project(jsrf_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(jsrf_runtime STATIC
    src/media_root.cpp
    src/binary_file.cpp
    src/tagged_container.cpp
    src/startup_validation.cpp
    src/asset_container.cpp
    src/bc1_texture.cpp
    src/texture_asset.cpp
    src/mdlb_mesh.cpp
    src/asset_export.cpp
    src/character_motion.cpp
    src/prototype_game.cpp
    src/prototype_character.cpp
    src/software_renderer.cpp
    src/prototype_scene.cpp
    src/stage_runtime.cpp
    src/player_grind.cpp
    src/player_locomotion.cpp
    src/stage_visual.cpp
)
target_include_directories(jsrf_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(jsrf_native src/main.cpp)
target_link_libraries(jsrf_native PRIVATE jsrf_runtime)

add_executable(test_media_root tests/test_media_root.cpp)
target_link_libraries(test_media_root PRIVATE jsrf_runtime)
add_test(NAME media_root COMMAND test_media_root)

add_executable(test_tagged_container tests/test_tagged_container.cpp)
target_link_libraries(test_tagged_container PRIVATE jsrf_runtime)
add_test(NAME tagged_container COMMAND test_tagged_container)

add_executable(test_retail_media tests/test_retail_media.cpp)
target_link_libraries(test_retail_media PRIVATE jsrf_runtime)
add_test(NAME retail_media COMMAND test_retail_media)

add_executable(test_asset_container tests/test_asset_container.cpp)
target_link_libraries(test_asset_container PRIVATE jsrf_runtime)
add_test(NAME asset_container COMMAND test_asset_container)

add_executable(test_bc1_texture tests/test_bc1_texture.cpp)
target_link_libraries(test_bc1_texture PRIVATE jsrf_runtime)
add_test(NAME bc1_texture COMMAND test_bc1_texture)

add_executable(test_retail_assets tests/test_retail_assets.cpp)
target_link_libraries(test_retail_assets PRIVATE jsrf_runtime)
add_test(NAME retail_assets COMMAND test_retail_assets)

add_executable(test_mdlb_mesh tests/test_mdlb_mesh.cpp)
target_link_libraries(test_mdlb_mesh PRIVATE jsrf_runtime)
add_test(NAME mdlb_mesh COMMAND test_mdlb_mesh)

add_executable(test_exporters tests/test_exporters.cpp)
target_link_libraries(test_exporters PRIVATE jsrf_runtime)
add_test(NAME exporters COMMAND test_exporters)

add_executable(test_retail_characters tests/test_retail_characters.cpp)
target_link_libraries(test_retail_characters PRIVATE jsrf_runtime)
add_test(NAME retail_characters COMMAND test_retail_characters)

add_executable(test_character_motion tests/test_character_motion.cpp)
target_link_libraries(test_character_motion PRIVATE jsrf_runtime)
add_test(NAME character_motion COMMAND test_character_motion)

add_executable(test_retail_motion tests/test_retail_motion.cpp)
target_link_libraries(test_retail_motion PRIVATE jsrf_runtime)
add_test(NAME retail_motion COMMAND test_retail_motion)

add_executable(test_prototype_game tests/test_prototype_game.cpp)
target_link_libraries(test_prototype_game PRIVATE jsrf_runtime)
add_test(NAME prototype_game COMMAND test_prototype_game)

add_executable(test_prototype_character tests/test_prototype_character.cpp)
target_link_libraries(test_prototype_character PRIVATE jsrf_runtime)
add_test(NAME prototype_character COMMAND test_prototype_character)

add_executable(test_software_renderer tests/test_software_renderer.cpp)
target_link_libraries(test_software_renderer PRIVATE jsrf_runtime)
add_test(NAME software_renderer COMMAND test_software_renderer)

add_executable(test_prototype_scene tests/test_prototype_scene.cpp)
target_link_libraries(test_prototype_scene PRIVATE jsrf_runtime)
add_test(NAME prototype_scene COMMAND test_prototype_scene)

add_executable(test_stage_runtime tests/test_stage_runtime.cpp)
target_link_libraries(test_stage_runtime PRIVATE jsrf_runtime)
add_test(NAME stage_runtime COMMAND test_stage_runtime)

add_executable(test_player_grind tests/test_player_grind.cpp)
target_link_libraries(test_player_grind PRIVATE jsrf_runtime)
add_test(NAME player_grind COMMAND test_player_grind)

add_executable(test_player_locomotion tests/test_player_locomotion.cpp)
target_link_libraries(test_player_locomotion PRIVATE jsrf_runtime)
add_test(NAME player_locomotion COMMAND test_player_locomotion)

add_executable(jsrf_prototype
    src/prototype_main.cpp
)
target_link_libraries(jsrf_prototype PRIVATE jsrf_runtime)
if(WIN32)
    target_sources(jsrf_prototype PRIVATE src/prototype_window_win32.cpp)
    target_link_libraries(jsrf_prototype PRIVATE user32 gdi32)
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_sources(jsrf_prototype PRIVATE src/prototype_window_x11.cpp)
    target_include_directories(jsrf_prototype PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(jsrf_prototype PRIVATE ${X11_LIBRARIES})
endif()

add_executable(test_stage_visual tests/test_stage_visual.cpp)
target_link_libraries(test_stage_visual PRIVATE jsrf_runtime)
add_test(NAME stage_visual COMMAND test_stage_visual)
