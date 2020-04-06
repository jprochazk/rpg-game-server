

set(
    ENV{SERVER_SOURCES}
    $ENV{SERVER_SOURCES}
    ${SERVER_SOURCE_DIR}/network/socket_listener.cpp
    ${SERVER_SOURCE_DIR}/network/socket_manager.cpp
    ${SERVER_SOURCE_DIR}/network/websocket_session.cpp
)