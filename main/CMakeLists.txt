idf_component_register(SRCS "main.cpp" "max30102.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver nvs_flash)

set(CMAKE_CXX_STANDARD 11)