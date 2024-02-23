
# debug/release
CFG = ""
CONFIG(debug, debug|release){
    CFG = "debug"
    TARGET = $$TARGET"d"
}

CONFIG(release, debug|release){
    CFG = "release"
}

# CONFIG
CONFIG += warn_on

# OS
COMPILER = ""   # vs / gcc
OS = ""         # Windows / Linux
EXT_O = ""      # .obj / .o

win32:{
    OS = "Windows"
    COMPILER = "vs"
    EXT_O = ".obj"
}

unix:!macx{
    OS = "Linux"
    COMPILER = "gcc"
    EXT_O = ".o"
}

# equals(QT_MAJOR_VERSION, 6){
# CONFIG+=c++23
# CONFIG += c++20
CONFIG += c++latest
# }

# ARCH
equals(COMPILER, "gcc"){ # GCC (TODO)
    # FLAGS
    QMAKE_CXXFLAGS += -fopenmp -O3
    QMAKE_LFLAGS   += -fopenmp
}

equals(COMPILER, "vs"){ # Visual studio

    # QMAKE_CXXFLAG *= -std:c++latest

    # set cl flags
    # equals(QT_MAJOR_VERSION, 5){
    #     QMAKE_CXXFLAGS *= -std:c++20    # The latest draft C++ standard preview features.
    # }

    QMAKE_CXXFLAGS *= -MP               # Build with Multiple Processes

    # warnings levels
    QMAKE_CXXFLAGS -= -W0
    QMAKE_CXXFLAGS -= -W1
    QMAKE_CXXFLAGS -= -W2
    QMAKE_CXXFLAGS *= -W3
    # QMAKE_CXXFLAGS *= -W4
    # QMAKE_CXXFLAGS *= -WX
    # optimization
    QMAKE_CXXFLAGS -= -O1               # Creates small code.
    QMAKE_CXXFLAGS *= -O2               # Creates fast code.

    #cl -c
    #-nologo:               Suppresses the display of the copyright banner when the compiler starts up and display of informational messages during compiling.
    #-Zc:wchar_t:           Parse wchar_t as a built-in type according to the C++ standard.
    #-FS:                   Forces writes to the PDB file to be serialized through MSPDBSRV.EXE.
    #-Zc:rvalueCast:        Enforce Standard C++ explicit type conversion rules (off by default).
    #-Zc:inline             Remove unreferenced functions or data if they're COMDAT or have internal linkage only (off by default).
    #-Zc:strictStrings      Disable string-literal to char* or wchar_t* conversion (off by default).
    #-Zc:throwingNew        Assume operator new throws on failure (off by default).
    #-Zc:referenceBinding   A UDT temporary won't bind to a non-const lvalue reference (off by default).
    #-Zc:__cplusplus        Enable the __cplusplus macro to report the supported standard (off by default).
    #-O1                    Creates small code.
    #-O2                    Creates fast code.
    #/std:c++latest         The latest draft C++ standard preview features.
    #/openmp                Enables #pragma omp in source code.
    #-MP                    Build with Multiple Processes
    #-MD                    Compiles to create a multithreaded DLL, by using MSVCRT.lib.
}




