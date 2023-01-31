set(ENGINE_ROOT_DIR "${CHAOTIRENDER_ROOT_DIR}/chaotirender")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")

add_subdirectory(3rdparty)
add_subdirectory(engine/runtime)
add_subdirectory(engine/editor)