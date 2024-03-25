add_executable(${PROJECT_NAME} "main.cpp" ${HEADERS} ${SOURCES})

if (MSVC)
    set_target_properties(
            ${PROJECT_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/")
endif ()

file(COPY ../assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../assets/)

target_link_libraries(${PROJECT_NAME} PUBLIC
        Vivid)