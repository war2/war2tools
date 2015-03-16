if (APPLE)
   find_program(XCODEBUILD xcodebuild)
   if (XCODEBUILD)
      add_custom_target(
         quicklook
         COMMAND xcodebuild -scheme Pud -project ${CMAKE_CURRENT_SOURCE_DIR}/Pud.xcodeproj/ CONFIGURATION_BUILD_DIR='${CMAKE_CURRENT_BINARY_DIR}'
      )
   endif()
endif()

