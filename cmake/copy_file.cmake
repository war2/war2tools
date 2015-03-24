function(copy_file file_src)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${file_src} ${CMAKE_CURRENT_BINARY_DIR}/${file_src} COPYONLY)
endfunction()

