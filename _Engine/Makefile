VCPKG 		= C:\vcpkg\installed\x64-windows

CXX			= cl
LIB			= lib	
CXXFLAGS	= /nologo /EHsc /std:c++20 /MD /Zi /Od  /Fdoutput\Ankaria.pdb /Iincludes /I$(VCPKG)\include

TARGET		= output\Ankaria.lib

SOURCES		= sources\Ankaria.cpp \
			  sources\Backend.cpp \
			  sources\Entity.cpp \
			  sources\Events.cpp \
			  sources\Renderer.cpp \
			  sources\Resources.cpp \
			  sources\Scene.cpp \
			  sources\Scenes.cpp
					
OBJECTS		= objects\Ankaria.obj \
			  objects\Backend.obj \
			  objects\Entity.obj \
			  objects\Events.obj \
			  objects\Renderer.obj \
			  objects\Resources.obj \
			  objects\Scene.obj \
			  objects\Scenes.obj

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LIB) /nologo /out:$(TARGET) $(OBJECTS)

$(OBJECTS):
	$(CXX) $(CXXFLAGS) /c sources\$(@B).cpp /Fo$@

clean:
	-del /Q objects\Ankaria.obj
	-del /Q objects\Backend.obj
	-del /Q objects\Entity.obj
	-del /Q objects\Events.obj
	-del /Q objects\Renderer.obj
	-del /Q objects\Resources.obj
	-del /Q objects\Scene.obj
	-del /Q objects\Scenes.obj
	-del /Q output\Ankaria.lib

re: clean all