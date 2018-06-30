CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
PROJECT(sqltoast)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
INCLUDE(GNUInstallDirs)

ADD_SUBDIRECTORY(libsqltoast)
ADD_SUBDIRECTORY(sqltoaster)
