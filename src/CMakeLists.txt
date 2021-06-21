
set(KUBOT_SOURCES
    main.cpp
    Kubot.h
    Kubot.cpp
    Utils.h
    Utils.cpp
    BuildOrderExecutor.h
    BuildOrderExecutor.cpp
    CannonRush.h
    CannonRush.cpp
    Drawer.h
    MacroManager.h
    MacroManager.cpp
    EventListener.h
    SC2.h
    SC2.cpp
    utils/BuildingPlacer.h
    utils/BuildingPlacer.cpp
    utils/Grid.h
    utils/GridUtils.h
    utils/GridView.cpp
    utils/GridView.h
    utils/Map.cpp
    utils/Map.h
    utils/UnitQuery.cpp
    utils/UnitQuery.h
    utils/UnitTraits.cpp
    utils/UnitTraits.h
    utils/Footprints.h
    )

add_executable(kubot ${KUBOT_SOURCES})

target_include_directories(kubot PUBLIC
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/s2client-api/include
    ${CMAKE_SOURCE_DIR}/external/s2client-api/protocol
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/external/s2client-api/generated
    )

target_link_libraries(kubot
    sc2api sc2lib sc2utils sc2protocol
    )
