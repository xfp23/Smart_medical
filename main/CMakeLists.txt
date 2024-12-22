idf_component_register(SRCS "http.cpp" "MAX30102.cpp" "main.cpp" "System.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver nvs_flash esp_http_server  esp_wifi 
                    )

set(CMAKE_CXX_STANDARD 11)