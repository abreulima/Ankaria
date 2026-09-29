VCPKG 		= C:\vcpkg\installed\x64-windows

CXX			= cl
CXXFLAGS	= /nologo /EHsc /std:c++20 /Zi /Od /MD /Iincludes /I$(ENGINE_DIR)\includes /I$(VCPKG)\include
LDFLAGS		= /link /DEBUG /PDB:output\win\Game.pdb /LIBPATH:$(VCPKG)\lib

TARGET		= output\win\Game.exe
PDB			= objects\wib\Game.pdb
SOURCES		= sources\Game.cpp
OBJECTS		= objects\Game.obj


ENGINE_DIR 	= ..\_Engine

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) /Fe:$(TARGET) $(ENGINE_DIR)\output\Ankaria.lib $(LDFLAGS) SDL3.lib SDL3_ttf.lib webgpu_dawn.lib

$(OBJECTS): $(SOURCES)
	$(CXX) $(CXXFLAGS) /c $(SOURCES) /Fo$@

clean:
	del /Q $(OBJECTS)
	del /Q $(TARGET)

run: all
	output\win\game.exe

re: clean all