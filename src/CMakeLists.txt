set(SOURCES
    main.cpp
    gamewindow.cpp
    chessboardwidget.cpp
    gamecontroller.cpp
    boardstate.cpp
    aiengine.cpp
    networkmanager.cpp
    scoreboard.cpp
)

set(HEADERS
    gamewindow.h
    chessboardwidget.h
    gamecontroller.h
    boardstate.h
    aiengine.h
    networkmanager.h
    scoreboard.h
)

add_executable(chinese_chess ${SOURCES} ${HEADERS})
target_link_libraries(chinese_chess PRIVATE Qt6::Widgets Qt6::Network)

target_include_directories(chinese_chess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
