# JJ2REPL GCC Makefile

CXX = g++
CXXFLAGS_RELEASE = -IG:/steve/angelscript_2.37.0/angelscript/include -O3
CXXFLAGS_DEBUG = -IG:/steve/angelscript_2.37.0/angelscript/include -g -O0
SRCDIR = ../source
OBJDIR_RELEASE = obj/release
OBJDIR_DEBUG = obj/debug
BIN_DIR = ../bin

SRCNAMES = main.cpp funcs.cpp runtime.cpp

# Object files for release and debug
OBJ_RELEASE = $(addprefix $(OBJDIR_RELEASE)/, $(notdir $(SRCNAMES:.cpp=.o))) obj/scriptstdstring.o obj/scriptbuilder.o obj/scriptstdstring_utils.o obj/scriptarray.o obj/scriptdictionary.o obj/scriptmath.o obj/scriptmathcomplex.o obj/scripthelper.o obj/scriptany.o
OBJ_DEBUG = $(addprefix $(OBJDIR_DEBUG)/, $(notdir $(SRCNAMES:.cpp=.o))) obj/scriptstdstring.o obj/scriptbuilder.o obj/scriptstdstring_utils.o obj/scriptarray.o obj/scriptdictionary.o obj/scriptmath.o obj/scriptmathcomplex.o obj/scripthelper.o obj/scriptany.o

BIN_RELEASE = $(BIN_DIR)/main_release.exe
BIN_DEBUG = $(BIN_DIR)/main_debug.exe
DELETER = rm -f

# Default target
all: debug

# Release build target
release: CXXFLAGS = $(CXXFLAGS_RELEASE)
release: $(BIN_RELEASE)

# Debug build target
debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: $(BIN_DEBUG)

# Build release executable
$(BIN_RELEASE): $(OBJ_RELEASE)
	$(CXX) -o $(BIN_RELEASE) $(OBJ_RELEASE) -static -langelscript -dynamic -L G:/steve/angelscript_2.37.0/angelscript/lib -lpthread
	@echo -------------------------------------------------------------------
	@echo Release build done.

# Build debug executable
$(BIN_DEBUG): $(OBJ_DEBUG)
	$(CXX) -o $(BIN_DEBUG) $(OBJ_DEBUG) -static -langelscript -dynamic -L G:/steve/angelscript_2.37.0/angelscript/lib -lpthread
	@echo -------------------------------------------------------------------
	@echo Debug build done.

# Compile source files for release
$(OBJDIR_RELEASE)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

# Compile source files for debug
$(OBJDIR_DEBUG)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

# Specific object files
obj/scriptstdstring.o: G:/steve/angelscript_2.37.0/add_on/scriptstdstring/scriptstdstring.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
obj/scriptstdstring_utils.o: G:/steve/angelscript_2.37.0/add_on/scriptstdstring/scriptstdstring_utils.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/scriptbuilder.o: G:/steve/angelscript_2.37.0/add_on/scriptbuilder/scriptbuilder.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
obj/scripthelper.o: G:/steve/angelscript_2.37.0/add_on/scripthelper/scripthelper.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/scriptarray.o: G:/steve/angelscript_2.37.0/add_on/scriptarray/scriptarray.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
obj/scriptdictionary.o: G:/steve/angelscript_2.37.0/add_on/scriptdictionary/scriptdictionary.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/scriptmath.o: G:/steve/angelscript_2.37.0/add_on/scriptmath/scriptmath.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
obj/scriptmathcomplex.o: G:/steve/angelscript_2.37.0/add_on/scriptmath/scriptmathcomplex.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/scriptany.o: G:/steve/angelscript_2.37.0/add_on/scriptany/scriptany.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(DELETER) $(OBJDIR_RELEASE) $(OBJDIR_DEBUG) $(BIN_RELEASE) $(BIN_DEBUG)

.PHONY: all clean release debug