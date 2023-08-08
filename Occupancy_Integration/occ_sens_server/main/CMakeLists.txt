set(srcs
       
        "board_server.c"
        "sensor_uart_bi.c"
        "../../shared/integratedTrackingAlgorithm.c"
        "../../shared/conversions.c"
)

idf_component_register(SRCS "main.c" "sensor_server.c" "${srcs}"
                        INCLUDE_DIRS "." "../../shared")
