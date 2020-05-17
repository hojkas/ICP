# ICP projekt: Makefile
# xstrna14, xlebod00

SOURCE_DIR = src/icp
EXAMPLE_DIR = examples/
SRC_FILES = $(SOURCE_DIR)/backend.cpp \
		   	$(SOURCE_DIR)/backend.h \
		    $(SOURCE_DIR)/connectionHandler.cpp \
		    $(SOURCE_DIR)/connectionHandler.h \
			$(SOURCE_DIR)/main.cpp \
			$(SOURCE_DIR)/mainwindow.cpp \
			$(SOURCE_DIR)/mainwindow.h \
			$(SOURCE_DIR)/mapwidget.cpp \
			$(SOURCE_DIR)/mapwidget.h \
			$(SOURCE_DIR)/street.cpp \
			$(SOURCE_DIR)/street.h 
JSON_FILES = $(EXAMPLE_DIR)/connections.json \
				$(EXAMPLE_DIR)/map.json

build:
	@qmake -makefile -o $(SOURCE_DIR)/Makefile $(SOURCE_DIR)/icp.pro >/dev/null
	@cd $(SOURCE_DIR) && make all >/dev/null

run:
	@cd $(SOURCE_DIR) && ./icp

doxygen:
	doxygen doxyConfig

clean:
	@cd $(SOURCE_DIR) && [ -f Makefile ] && [ -f main.o ] && make -s clean || :
	@rm -f $(SOURCE_DIR)/icp
	@rm -f $(SOURCE_DIR)/Makefile

pack:
	@zip xlebod00_xstrna14.zip Makefile README.txt doxyConfig $(JSON_FILES) $(SRC_FILES)
.PHONY: build run doxygen clean pack