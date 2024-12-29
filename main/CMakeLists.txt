idf_component_register(SRCS  "http.cpp" "MAX30102.cpp" "main.cpp" "System.cpp" "index.cpp"  "i2c.cpp" "timer.cpp" "oled.cpp" "NTC.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver nvs_flash esp_http_server  esp_wifi spiffs esp_http_client esp_timer esp_adc
                    )

set(CMAKE_CXX_STANDARD 11)