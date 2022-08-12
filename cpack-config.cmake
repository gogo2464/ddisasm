set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

set(CPACK_DDISASM_DEB_VERSION
    "${CPACK_DDISASM_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}")
set(CPACK_GTIRB_PPRINTER_DEB_VERSION
    "${CPACK_GTIRB_PPRINTER_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}")
set(CPACK_GTIRB_DEB_VERSION
    "${CPACK_GTIRB_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}")

set(CPACK_DDISASM_SUFFIX "")
set(CPACK_GTIRB_PPRINTER_SUFFIX "")
set(CPACK_GTIRB_SUFFIX "")
set(CPACK_CAPSTONE_SUFFIX "")

# Debian packages
if(CPACK_DDISASM_STABLE_PKG_NAME)
  string(REGEX REPLACE "^[0-9]+:" "" CPACK_CAPSTONE_SUFFIX
                       "${CPACK_CAPSTONE_PKG_VERSION}")
  set(CPACK_DDISASM_SUFFIX "-${CPACK_DDISASM_VERSION}")
  set(CPACK_GTIRB_PPRINTER_SUFFIX "-${CPACK_GTIRB_PPRINTER_VERSION}")
  set(CPACK_GTIRB_SUFFIX "-${CPACK_GTIRB_VERSION}")
  set(CPACK_CAPSTONE_SUFFIX "-${CPACK_CAPSTONE_SUFFIX}")
endif()
if("${CPACK_DDISASM_PACKAGE}" STREQUAL "deb-ddisasm")
  set(CPACK_DEBIAN_PACKAGE_NAME "ddisasm${CPACK_DDISASM_SUFFIX}")
  set(CPACK_PACKAGE_FILE_NAME "ddisasm")
  set(CPACK_COMPONENTS_ALL ddisasm)
  if("${CPACK_DEBIAN_PACKAGE_RELEASE}" STREQUAL "focal")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libstdc++6, libc6, libgcc1, libgomp1, libgtirb${CPACK_GTIRB_SUFFIX} (=${CPACK_GTIRB_DEB_VERSION}), libgtirb-pprinter${CPACK_GTIRB_PPRINTER_SUFFIX} (=${CPACK_GTIRB_PPRINTER_DEB_VERSION}), libboost-filesystem1.71.0, libboost-program-options1.71.0, libcapstone-dev${CPACK_CAPSTONE_SUFFIX} (=${CPACK_CAPSTONE_PKG_VERSION})"
    )
  else()
    message(
      SEND_ERROR "Uknown / missing value for CPACK_DEBIAN_PACKAGE_RELEASE.")
  endif()
elseif("${CPACK_DDISASM_PACKAGE}" STREQUAL "deb-debug")
  set(CPACK_DEBIAN_PACKAGE_NAME "ddisasm-dbg${CPACK_DDISASM_SUFFIX}")
  set(CPACK_PACKAGE_FILE_NAME "ddisasm-dbg")
  set(CPACK_COMPONENTS_ALL debug-file)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS
      "ddisasm${CPACK_DDISASM_SUFFIX} (=${CPACK_DDISASM_DEB_VERSION})")
endif()
