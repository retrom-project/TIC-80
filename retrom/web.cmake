add_executable(tic80-retrom retrom/bridge.c)
target_link_libraries(tic80-retrom PRIVATE tic80core)
set_target_properties(tic80-retrom PROPERTIES SUFFIX ".mjs")
target_link_options(tic80-retrom PRIVATE
    "-sMODULARIZE=1" "-sEXPORT_ES6=1" "-sENVIRONMENT=web,node"
    "-sALLOW_MEMORY_GROWTH=1" "-sMAXIMUM_MEMORY=268435456"
    "-sEXPORTED_FUNCTIONS=['_malloc','_free']"
    "-sEXPORTED_RUNTIME_METHODS=['UTF8ToString','HEAPU8','HEAP16']")
