idf_component_register(SRCS "MAX30102.cpp" "main.cpp" "max30102.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver nvs_flash esp_http_server  esp_wifi 
                    )

set(CMAKE_CXX_STANDARD 11)