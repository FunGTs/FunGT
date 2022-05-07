BIN = bin
OBJ_DIR = obj
RED = \033[1;31m
GREEN = \033[1;32m
BLUE = \033[1;34m
YELLOW = \033[1;33m
NC = \033[1;0m
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
LIB = -I/usr/local/include -L/usr/local/lib -lglfw -framework Cocoa -framework OpenGL -framework IOKit -ldl
CC = c++-11
TARGET = $(BIN)/funGT_macos.exe
endif
ifeq ($(UNAME), Linux)
LIB = -lGL -lGLU -lglut -lglfw -lGLEW -ldl
TARGET = $(BIN)/funGL.exe
CC = c++
endif

#************ CLASS DIR ***********

DISP = ../Display
SHADER = ../Shaders
VERTEX = ../VertexGL
TEXTURES = ../Textures
STB = ../vendor/stb_image
IMGUI = ../vendor/imgui
IMGUI_SET = ../Imgui_Setup
# ********** SOURCE *************

SOURCE += main.cpp
SOURCE += $(wildcard $(VERTEX)/*.cpp)
SOURCE += $(SHADER)/shader.cpp
SOURCE += $(DISP)/display.cpp
SOURCE += $(TEXTURES)/textures.cpp
SOURCE += $(STB)/stb_image.cpp
SOURCE += $(IMGUI)/imgui.cpp 
SOURCE += $(IMGUI)/imgui_demo.cpp $(IMGUI)/imgui_draw.cpp 
SOURCE += $(IMGUI)/imgui_tables.cpp $(IMGUI)/imgui_widgets.cpp
SOURCE += $(IMGUI)/imgui_impl_glfw.cpp $(IMGUI)/imgui_impl_opengl3.cpp
SOURCE += $(IMGUI_SET)/imgui_setup.cpp

#****** HEADERS ************
INV += $(wildcard $(VERTEX)/*.hpp)
INC += $(SHADER)/shader.hpp
INC += $(DISP)/display.hpp
INC += $(TEXTURES)/textures.hpp
INC += $(STB)/stb_image.h
INC += $(IMGUI_SET)/imgui_setup.hpp


#**********
VPATH = $(SHADER) $(VERTEX) $(TEXTURES) $(DISP) $(STB) $(IMGUI) $(IMGUI_SET)

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

