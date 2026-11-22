CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra -pedantic-errors
GTEST_DIR = googletest/googletest
INCLUDES = -I. -isystem $(GTEST_DIR)/include -isystem $(GTEST_DIR)

TARGET = graph_tests

.PHONY: all test clean

all: $(TARGET)

PM_SRCS = src/PackageManager/Domain/Version.cpp \
          src/PackageManager/Domain/Requirement.cpp \
          src/PackageManager/Domain/Package.cpp \
          src/PackageManager/Catalog/PackageCatalog.cpp \
          src/PackageManager/Storage/InstalledRegistry.cpp

$(TARGET): tests.cpp $(PM_SRCS) $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests.cpp $(PM_SRCS) $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc -pthread -o $(TARGET)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) *.o *.dSYM
