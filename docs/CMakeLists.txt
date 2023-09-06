
find_package(Doxygen REQUIRED doxygen)

set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")

doxygen_add_docs(docs 
        ${CMAKE_SOURCE_DIR}/src/app  
        ${CMAKE_SOURCE_DIR}/src/lib 
        ALL 
        COMMENT "Generate HTML docs")		


