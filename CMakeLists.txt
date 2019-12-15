cmake_minimum_required(VERSION 3.15)
project(CTPAPI VERSION 0.1 DESCRIPTION "ctpaip")

set(CMAKE_CXX_STANDARD 17)

# include(GNUInstallDirs)
include_directories("/usr/include")
include_directories("/usr/local/include")
include_directories("include")
include_directories("v6315_20190220")

# add_library(libuv SHARED IMPORTED)
# set_target_properties(libuv PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libuv.so")
# add_library(ctp_mdapi SHARED IMPORTED)
# set_target_properties(ctp_mdapi PROPERTIES IMPORTED_LOCATION "/usr/lib/libthostmduserapi_se.so")
# add_library(ctp_tdapi SHARED IMPORTED)
# set_target_properties(ctp_tdapi PROPERTIES IMPORTED_LOCATION "/usr/lib/libthosttraderapi_se.so")


# set portfolio
add_library(portfolio SHARED include/portfolio.h src/portfolio.cpp include/portfolio_global.h include/mylog.h
            src/mylog.c include/str.h src/str.c include/mem.h src/mem.cpp include/ptf_allocator.h src/ptf_allocator.cpp)
target_compile_definitions(portfolio PRIVATE PORTFOLIO_LIBRARY _UNIX)
# target_link_libraries(portfolio libuv)
set_target_properties(portfolio PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER include/portfolio.h)
# configure_file(portfolio.pc.in portfolio.pc @ONLY)
# install(TARGETS portfolio
#         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# install(FILES ${CMAKE_BINARY_DIR}/portfolio.pc
#         DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)


# add_library(ctpapi SHARED src/ctpapi.cpp include/ctpapi.h)
# target_compile_definitions(ctpapi PRIVATE CTPAPI_LIBRARY _UNIX)
# target_link_libraries(ctpapi ctp_mdapi ctp_tdapi portfolio)
# # set ctpapi
# set_target_properties(ctpapi PROPERTIES
#         VERSION ${PROJECT_VERSION}
#         SOVERSION 1
#         PUBLIC_HEADER include/ctpapi.h)
# # configure_file(ctpapi.pc.in ctpapi.pc @ONLY)
# # install(TARGETS ctpapi
# #         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# #         PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# # install(FILES ${CMAKE_BINARY_DIR}/ctpapi.pc
# #         DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)



# set demo
add_executable(demo src/main.cpp)
target_compile_definitions(demo PRIVATE _UNIX)
# target_link_libraries(demo ctpapi portfolio)
target_link_libraries(demo portfolio)


# set lcpp (learn cpp)
add_executable(lcpp src/lcpp.cpp)
target_compile_definitions(lcpp PRIVATE _UNIX)
target_link_libraries(lcpp portfolio)