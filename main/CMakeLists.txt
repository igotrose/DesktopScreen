idf_component_register(SRCS 
                        "main.c"
                        "src/hal/ds_timer.c"
                        "src/fs/ds_spiffs.c"
                    INCLUDE_DIRS 
                        "."
                        "inc/hal"
                        "inc/fs")