# This file is part of kdegraphics.

# kdegraphics is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# kdegraphics is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with kdegraphics.  If not, see <https://www.gnu.org/licenses/>.

# (C) 2018 by Helio Chissini de Castro <helio@kde.org>

cmake_minimum_required(VERSION 3.1.0)

project(kdegraphics VERSION 2.2.2 LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(kde2_configure)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(JPEG)

find_package(Qt2 REQUIRED)

find_package(DCOP REQUIRED)

find_package(kdelibs2 REQUIRED)
find_package(kdecore REQUIRED)
find_package(kdeui REQUIRED)
find_package(kdesu REQUIRED)
find_package(kssl REQUIRED)
find_package(kio REQUIRED)
find_package(ksycoca REQUIRED)
find_package(kfile REQUIRED)
find_package(kparts REQUIRED)
find_package(mcop REQUIRED)
find_package(qtmcop REQUIRED)
find_package(artsflow REQUIRED)
find_package(kmedia2 REQUIRED)
find_package(artskde REQUIRED)
find_package(kdeprint REQUIRED)
find_package(kjava REQUIRED)
find_package(khtml REQUIRED)
find_package(kdefakes REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(kscreensaver REQUIRED)

find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

add_definitions(-DQT_CLEAN_NAMESPACE -DQT_NO_COMPAT -DQT_NO_ASCII_CAST)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
create_kde2_config_header()

#add_subdirectory(doc)
#add_subdirectory(kamera)
add_subdirectory(kcoloredit)
#add_subdirectory(kdvi)
#add_subdirectory(kfax)
#add_subdirectory(kfaxview)
#add_subdirectory(kfract)
#add_subdirectory(kghostview)
#add_subdirectory(kiconedit)
#add_subdirectory(kooka)
add_subdirectory(kpaint)
add_subdirectory(kruler)
add_subdirectory(ksnapshot)
add_subdirectory(kview)
add_subdirectory(kviewshell)
add_subdirectory(pixie)
add_subdirectory(libminimagick)
#add_subdirectory(libkscan)

