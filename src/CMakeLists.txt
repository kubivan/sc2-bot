

set(KUBOT_SOURCES
    main.cpp
	Kubot.h
	Kubot.cpp
	Utils.h
	Utils.cpp
	CannonRush.h
	CannonRush.cpp
	MacroManager.h
	MacroManager.cpp
	EventListener.h
	SC2.h
	SC2.cpp
	MapSegmentation.h
	MapSegmentation.cpp
	utils/GridView.h
	utils/GridView.cpp
	utils/Grid.h
	utils/GridUtils.h
	utils/UnitQuery.h
	utils/UnitQuery.cpp
	utils/UnitTraits.h
	utils/UnitTraits.cpp
	)

add_executable(kubot ${KUBOT_SOURCES})

target_include_directories(kubot PUBLIC
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/s2client-api/include
    ${CMAKE_SOURCE_DIR}/external/s2client-api/protocol
    ${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/external/s2client-api/generated
    )

target_compile_features(kubot PUBLIC cxx_std_14)
target_link_libraries(kubot
    sc2api sc2lib sc2utils sc2protocol
    )
