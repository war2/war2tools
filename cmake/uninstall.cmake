set(INSTALL_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt")

if (NOT EXISTS ${INSTALL_MANIFEST})
   message(FATAL_ERROR "Cannot find install manifest: ${INSTALL_MANIFEST}")
endif()

file (STRINGS ${INSTALL_MANIFEST} files)
foreach (file ${files})
   if (EXISTS ${file})
      message (STATUS "Removing file: '${file}'")
      exec_program (
         ${CMAKE_COMMAND} ARGS "-E remove ${file}"
         OUTPUT_VARIABLE stdout
         RETURN_VALUE result
         )
      if (NOT "${result}" STREQUAL 0)
         message (FATAL_ERROR "Failed to remove ${file}")
      endif()
   else()
      message (STATUS "File ${file} does not exist!")
   endif()

endforeach(file)

