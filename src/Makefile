CP := gcc
CFLAGS := -Wall -Werror -Wextra -std=c11
GCOVFLAGS := -L. --coverage
OS = $(shell uname)
ifeq ($(OS), Linux)
	CHECK_FLAGS = $(shell pkg-config --libs check) -lm
else
	CHECK_FLAGS = -lcheck
endif
SRC_BACKEND := $(wildcard backend/*.c)
HDR_BACKEND := $(wildcard backend/*.h)
SRC_QT := $(wildcard gui/gt_gui/*.cpp)
HDR_QT := $(wildcard gui/gt_gui/*.h)
DOC_PATH := ../documentation

all: test

install:
	- cd ..; mkdir build
	cd gui/gt_gui/; cmake .; make;
	cp -R gui/gt_gui/gt_gui.app ../build/SmartCalc.app
	chmod +rwx ../build
	chmod +rwx ../build/SmartCalc.app
	- open ../build/SmartCalc.app

uninstall:
	rm -rf ../build/*.app

clean:
	- rm -rf test .clang-format report gcov_report *.gcno *.gcda *.info 
	- rm -rf gui/gt_gui/CMakeCache.txt
	- rm -rf gui/gt_gui/CMakeFiles
	- rm -rf gui/gt_gui/Makefile
	- rm -rf gui/gt_gui/cmake_install.cmake
	- rm -rf gui/gt_gui/gt_gui_autogen
	- rm -rf gui/gt_gui/gt_gui.app
	- rm -rf ../archive
	- rm -rf gui/gt_gui/.qt
	- rm -rf gui/gt_gui/gt_gui
	- rm -rf $(DOC_PATH)

dist: install
	- rm -rf ../archive
	- mkdir ../archive
	- mkdir ../archive/SmartCalc_v1.0
	- cp -r ../build/SmartCalc.app ../archive/SmartCalc_v1.0
	- cp -r Manual ../archive/SmartCalc_v1.0
	- cd ../archive && tar cvzf Archive_SmartCalc_v1.0.tgz SmartCalc_v1.0 && rm -rf SmartCalc_v1.0

dvi:
	rm -rf $(DOC_PATH)
	mkdir $(DOC_PATH)
	doxygen Doxyfile

test:
	$(CP) $(CFLAGS) -o test $(SRC_BACKEND) $^ $(CHECK_FLAGS) -fsanitize=address
	./test

check:
	cp ../materials/linters/.clang-format .
	clang-format -n backend/*.c backend/*.h
	make test
	CK_FORK=no leaks --atExit -- ./test

gcov_report:
	$(CP) $(CFLAGS) $(SRC_BACKEND) $(CHECK_FLAGS) $(GCOVFLAGS) -o $@ 
	./gcov_report
	lcov -t gcov_report -o rep.info -c -d .
	genhtml -o report rep.info
	open ./report/index.html
	rm -rf *.gcda *.gcno *.info

clang-format:
	cp ../materials/linters/.clang-format .
	clang-format -n backend/*.c backend/*.h gui/gt_gui/*.h gui/gt_gui/*.cpp

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_BACKEND) $(HDR_BACKEND) --std=c11 --language=c
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_QT) $(HDR_QT) --std=c++17 --language=c++
