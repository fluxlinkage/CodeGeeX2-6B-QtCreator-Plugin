cmake_minimum_required(VERSION 3.10)

project(CodeGeeX2)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QtCreator REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
set(QtX Qt${QT_VERSION_MAJOR})

# Add a CMake option that enables building your plugin with tests.
# You don't want your released plugin binaries to contain tests,
# so make that default to 'NO'.
# Enable tests by passing -DWITH_TESTS=ON to CMake.
option(WITH_TESTS "Builds with tests" NO)

if(WITH_TESTS)
  # Look for QtTest
  find_package(${QtX} REQUIRED COMPONENTS Test)

  # Tell CMake functions like add_qtc_plugin about the QtTest component.
  set(IMPLICIT_DEPENDS Qt::Test)

  # Enable ctest for auto tests.
  enable_testing()
endif()

add_qtc_plugin(CodeGeeX2
  PLUGIN_DEPENDS
    QtCreator::Core
    QtCreator::LanguageClient
  DEPENDS
    ${QtX}::Widgets
    ${QtX}::Network
    QtCreator::ExtensionSystem
    QtCreator::Utils
    QtCreator::ProjectExplorer
    QtCreator::TextEditor
  SOURCES
    codegeex2.qrc
    codegeex2client.cpp
    codegeex2client.h
    codegeex2constants.h
    codegeex2hoverhandler.cpp
    codegeex2hoverhandler.h
    codegeex2icons.h
    codegeex2optionspage.cpp
    codegeex2optionspage.h
    codegeex2plugin.cpp
    codegeex2plugin.h
    codegeex2projectpanel.cpp
    codegeex2projectpanel.h
    codegeex2settings.cpp
    codegeex2settings.h
    codegeex2suggestion.cpp
    codegeex2suggestion.h
    codegeex2tr.h
    requests/getcompletions.h
    codegeex2clientinterface.h
    codegeex2clientinterface.cpp
)

qt_add_translations(CodeGeeX2 TS_FILES CodeGeeX2_zh_CN.ts)
