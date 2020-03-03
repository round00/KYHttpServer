set(base_src
        Logger.cpp
        util.cpp
        Mutex.cpp
        Condition.cpp
        Thread.cpp
        ConfigFile.cpp
        )

add_library(base ${base_src})
target_link_libraries(base pthread)
