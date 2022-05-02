BIN = bin
OBJ_DIR = obj
RED = \033[1;31m
GREEN = \033[1;32m
BLUE = \033[1;34m
YELLOW = \033[1;33m
NC = \033[1;0m
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
LIB = -framework Accelerate
CC = c++-7
TARGET = $(BIN)/funQC_macos.exe
endif
ifeq ($(UNAME), Linux)
LIB = -lGL -lGLU -lglut -lglfw -lGLEW
TARGET = $(BIN)/funGL.exe
CC = c++
endif

#************ CLASS DIR ***********

DISP = ../Display
SHADER = ../Shaders
VERTEX = ../VertexGL
TEXTURES = ../Textures
STB = ../vendor/stb_image

# ********** SOURCE *************

SOURCE += main.cpp
SOURCE += $(wildcard $(VERTEX)/*.cpp)
SOURCE += $(SHADER)/shader.cpp
SOURCE += $(DISP)/display.cpp
SOURCE += $(TEXTURES)/textures.cpp
SOURCE += $(STB)/stb_image.cpp
#****** HEADERS ************
INV += $(wildcard $(VERTEX)/*.hpp)
INC += $(SHADER)/shader.hpp
INC += $(DISP)/display.hpp
INC += $(TEXTURES)/textures.hpp
INC += $(STB)/stb_image.h


#**********
VPATH = $(SHADER) $(VERTEX) $(TEXTURES) $(DISP) $(STB)

OBJECT = $(patsubst %,$(OBJ_DIR)/%, $(notdir $(SOURCE:.cpp=.o)))
#OBJ = $(SOURCE:.cpp=.o)
$(TARGET) : $(OBJECT)
	@echo -e "$(RED)LINKING...$(NC)"
	$(CC) -o  $@ $^ -lm $(LIB)
	@echo -e "$(YELLOW)DONE!!!$(NC)"
	@echo -e "$(BLUE)OPERATING SYSTEM: $(UNAME)!!!...$(NC)"
$(OBJ_DIR)/%.o : %.cpp
	@echo -e "$(GREEN)COMPILING...$(NC)"
	@echo -e  "\033[0;32m [OK] \033[0m       \033[0;33m Compiling:\033[0m" $<
	$(CC) $(CFLAGS) $(LIB) -c  $< -o $@
print-%  : ; @echo $* = $($*)


.PHONY : help clean

clean :
	rm -f $(OBJECT) $(BIN)/$(TARGET)

