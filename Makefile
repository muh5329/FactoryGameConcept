COMPILER = cc
LIB_OPTS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
INCLUDE_PATHS = -Iinclude/
OUT = -o out
CFILES = *.c
PLATFORM := $(shell uname)

ifeq ($(PLATFORM), Darwin)
	COMPILER = clang
	BULLET_INCLUDE = -I/opt/homebrew/Cellar/bullet/3.25
	BULLET_LIBS = -L/opt/homebrew/Cellar/bullet/3.25 -lBulletDynamics -lBulletCollision -lLinearMath
	INCLUDE_PATHS += $(BULLET_INCLUDE)
	LIB_OPTS = $(BULLET_LIBS) -Llib/darwin/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lraylib
endif

build:
	$(COMPILER) $(INCLUDE_PATHS) $(CFILES) $(OUT) $(LIB_OPTS)

run:
	./out

clean:
	rm -rf ./out
