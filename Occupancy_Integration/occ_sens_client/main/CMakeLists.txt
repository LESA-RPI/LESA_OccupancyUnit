set(srcs
        "board_client.c"
        "sensor_client.c"
        "data_upload.c"
        "../../shared/conversions.c"
)

idf_component_register(SRCS "main.c" "${srcs}"
                    INCLUDE_DIRS  "." "../../shared")
