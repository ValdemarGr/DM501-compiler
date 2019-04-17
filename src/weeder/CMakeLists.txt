add_library(Weeder weeder.c weeds/weed_functions.c weeds/weed_constants.c weeds/weed_constants.h)

target_link_libraries(Weeder Error)
