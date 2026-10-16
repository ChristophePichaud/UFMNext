# CMake generated Testfile for 
# Source directory: C:/Dev/UFMNext
# Build directory: C:/Dev/UFMNext
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(CoreUnitTests "C:/Dev/UFMNext/Debug/ufmnext_tests.exe")
  set_tests_properties(CoreUnitTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Dev/UFMNext/CMakeLists.txt;158;add_test;C:/Dev/UFMNext/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(CoreUnitTests "C:/Dev/UFMNext/Release/ufmnext_tests.exe")
  set_tests_properties(CoreUnitTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Dev/UFMNext/CMakeLists.txt;158;add_test;C:/Dev/UFMNext/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(CoreUnitTests "C:/Dev/UFMNext/MinSizeRel/ufmnext_tests.exe")
  set_tests_properties(CoreUnitTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Dev/UFMNext/CMakeLists.txt;158;add_test;C:/Dev/UFMNext/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(CoreUnitTests "C:/Dev/UFMNext/RelWithDebInfo/ufmnext_tests.exe")
  set_tests_properties(CoreUnitTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Dev/UFMNext/CMakeLists.txt;158;add_test;C:/Dev/UFMNext/CMakeLists.txt;0;")
else()
  add_test(CoreUnitTests NOT_AVAILABLE)
endif()
subdirs("_deps/nlohmann_json-build")
