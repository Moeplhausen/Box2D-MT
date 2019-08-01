file(GLOB_RECURSE SOURCE_FILES
        "*.h"
        "*.cpp"
        )




add_library(Box2D-MT ${SOURCE_FILES})
