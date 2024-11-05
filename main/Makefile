#-std=c++17 -g -fsanitize=address
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
LIB = -lGL -lGLU -lglfw -lGLEW -ldl -lassimp
LDFLAGS = 
CFLAGS = -Wall 
ASSIMP_LIBS = -lassimp
TARGET = $(BIN)/funGL.exe
CC = g++-11
#CC = /home/juanchuletas/Documents/DeveloperZone/sycl_workspace/llvm/build/bin/clang++
endif

#************ CLASS DIR ***********

APP = ../funGT
SHADER = ../Shaders
VERTEX = ../VertexGL
TEXTURES = ../Textures
STB = ../vendor/stb_image
IMGUI = ../vendor/imgui
IMGUI_SET = ../Imgui_Setup
MATERIAL = ../Material
MESH = ../Mesh
GEOM = ../Geometries
CAM = ../Camera
GT = ../GT
MODEL = ../Model
AMODEL = ../AnimatedModel
HELP = ../Helpers
ANIMATION = ../Animation
BONE = ../Bone
MATRIX = ../Matrix
SCENE_MAN = ../SceneManager
CUBEMAP = ../CubeMap
RENDER = ../Renderable
DATAPATH = ../DataPaths
# ********** SOURCE *************

SOURCE += main.cpp
SOURCE += $(wildcard $(VERTEX)/*.cpp)
SOURCE += $(SHADER)/shader.cpp
SOURCE += $(App)/fungt.cpp
SOURCE += $(AMODEL)/animated_model.cpp
SOURCE += $(TEXTURES)/textures.cpp
SOURCE += $(STB)/stb_image.cpp
SOURCE += $(IMGUI)/imgui.cpp 
SOURCE += $(IMGUI)/imgui_demo.cpp $(IMGUI)/imgui_draw.cpp 
SOURCE += $(IMGUI)/imgui_tables.cpp $(IMGUI)/imgui_widgets.cpp
SOURCE += $(IMGUI)/imgui_impl_glfw.cpp $(IMGUI)/imgui_impl_opengl3.cpp
SOURCE += $(IMGUI_SET)/imgui_setup.cpp
SOURCE += $(MATERIAL)/material.cpp
SOURCE += $(MESH)/mesh.cpp
SOURCE += $(CAM)/camera.cpp
SOURCE += $(wildcard $(GEOM)/*.cpp)
SOURCE += $(MODEL)/model.cpp
SOURCE += $(HELP)/helpers.cpp
SOURCE += $(ANIMATION)/animation.cpp
SOURCE += $(BONE)/bone.cpp
SOURCE += $(MATRIX)/matrix4x4f.cpp
SOURCE += $(SCENE_MAN)/scene_manager.cpp
SOURCE += $(CUBEMAP)/cube_map.cpp


#****** HEADERS ************
INC += $(wildcard $(VERTEX)/*.hpp)
INC += $(SHADER)/shader.hpp
INC += $(APP)/fungt.hpp
INC += $(GT)/graphicsTool.hpp
INC += $(TEXTURES)/textures.hpp
INC += $(STB)/stb_image.h
INC += $(IMGUI_SET)/imgui_setup.hpp
INC += $(MATERIAL)/material.hpp
INC += $(MESH)/mesh.hpp
INC += $(CAM)/camera.hpp
INC += $(wildcard $(GEOM)/*.hpp)
INC += $(MODEL)/model.hpp
INC += $(AMODEL)/animated_modele.hpp
INC += $(HELP)/helpers.hpp
INC += $(ANIMATION)/animation.hpp
INC += $(BONE)/bone.hpp
INC += $(MATRIX)/matrix4x4f.hpp
INC += $(SCENE_MAN)/scene_manager.hpp
INC += $(CUBEMAP)/cube_map.hpp
INC += $(RENDERABLE)/renderable.hpp
INC += $(DATAPATH)/datapaths.hpp



#**********
VPATH = $(SHADER) $(VERTEX) $(TEXTURES) $(APP) $(STB) $(IMGUI) $(IMGUI_SET) $(MATERIAL) \
 	$(MESH) $(GEOM) $(CAM) $(GT) $(MODEL) $(AMODEL) $(HELP) $(ANIMATION) $(BONE) \
	$(MATRIX) $(SCENE_MAN) $(CUBEMAP) $(RENDERABLE) $(DATAPATH)

OBJECT = $(patsubst %,$(OBJ_DIR)/%, $(notdir $(SOURCE:.cpp=.o)))
#OBJ = $(SOURCE:.cpp=.o)
$(TARGET) : $(OBJECT)
	@echo -e "$(RED)LINKING...$(NC)"
	$(CC) -o  $@ $^ -lm $(LIB) $(LDFLAGS)
	@echo -e "$(YELLOW)DONE!!!$(NC)"
	@echo -e "$(BLUE)OPERATING SYSTEM: $(UNAME)!!!...$(NC)"
$(OBJ_DIR)/%.o : %.cpp
	@echo -e "$(GREEN)COMPILING...$(NC)"
	@echo -e  "\033[0;32m [OK] \033[0m       \033[0;33m Compiling:\033[0m" $<
	$(CC) $(CFLAGS) $(LIB) -c  $< -o $@
print-%  : ; @echo $* = $($*)


.PHONY : help clean
run :
	./funGL.exe

clean :
	rm -f $(OBJECT) $(BIN)/$(TARGET)

