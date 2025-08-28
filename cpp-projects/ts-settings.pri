
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
# CONFIG += warn_on

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

    # Build with Multiple Processes
    QMAKE_CXXFLAGS *= -MP

    # Enforces standards conformance.
    QMAKE_CXXFLAGS *= -permissive-

    # warnings
    QMAKE_CFLAGS_WARN_ON -= -W3
    QMAKE_CFLAGS_WARN_ON += -W4

    # QMAKE_CXXFLAGS *= -W4
    QMAKE_CXXFLAGS *= -w14640
    QMAKE_CXXFLAGS *= -w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss of data

    # QMAKE_CXXFLAGS *= -arch:AVX512
    # QMAKE_CXXFLAGS *= -openmp

    # /w14242 'identfier': conversion from 'type1' to 'type1', possible loss of data
    # /w14254 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
    # /w14263 'function': member function does not override any base class virtual member function
    # /w14265 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
    # /w14287 'operator': unsigned/negative constant mismatch
    # /we4289 nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
    # /w14296 'operator': expression is always 'boolean_value'
    # /w14311 'variable': pointer truncation from 'type1' to 'type2'
    # /w14545 expression before comma evaluates to a function which is missing an argument list
    # /w14546 function call before comma missing argument list
    # /w14547 'operator': operator before comma has no effect; expected operator with side-effect
    # /w14549 'operator': operator before comma has no effect; did you intend 'operator'?
    # /w14555 expression has no effect; expected expression with side-effect
    # /w14619 pragma warning: there is no warning number 'number'
    # /w14640 Enable warning on thread unsafe static member initialization
    # /w14826 Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
    # /w14905 wide string literal cast to 'LPSTR'
    # /w14906 string literal cast to 'LPWSTR'
    # /w14928 illegal copy-initialization; more than one user-defined conversion has been implicitly applied

    # QMAKE_CXXFLAGS *= -WX #  treat warnings as errors

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
    #-MD
}




