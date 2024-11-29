cmake_minimum_required(VERSION 3.0)
project(Game_Port)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile commands export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add compile options
add_compile_options(-g)

# GCC Diagnostics
add_compile_options(
    -fmax-errors=10  # Exit early when N syntax errors to limit screen fill
    -fno-asm
    -fno-builtin
    -fno-builtin-function
    -fenforce-eh-specs
    -fvisibility-inlines-hidden
    -fvisibility-ms-compat
)

# GCC Warnings
add_compile_options(
    # Grouped settings
    -Werror  # Converts all warnings to errors
    -Wextra
    -Wall

    # Single settings
    -Waddress
    -Waggressive-loop-optimizations
    -Wattributes
    -Wbool-compare
    -Wbuiltin-macro-redefined
    -Wcast-align
    -Wcatch-value=3
    -Wchar-subscripts
    -Wclass-conversion
    -Wclass-memaccess
    -Wclobbered
    -Wcomma-subscript
    -Wcomment
    -Wconditionally-supported
    -Wcoverage-mismatch
    -Wcpp
    -Wctad-maybe-unsupported
    -Wdangling-else
    -Wdate-time
    -Wdelete-incomplete
    -Wdeprecated-declarations
    -Wdeprecated-enum-enum-conversion
    -Wdeprecated-enum-float-conversion
    -Wdisabled-optimization
    -Wdiv-by-zero
    -Wempty-body
    -Wendif-labels
    -Wenum-compare
    -Wformat-contains-nul
    -Wformat-extra-args
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-signedness
    -Wformat-y2k
    -Wformat-zero-length
    -Wfree-nonheap-object
    -Wignored-qualifiers
    -Winherited-variadic-ctor
    -Winit-self
    -Winline
    -Wint-to-pointer-cast
    -Winvalid-imported-macros
    -Winvalid-memory-model
    -Winvalid-offsetof
    -Winvalid-pch
    -Wliteral-suffix
    -Wlogical-not-parentheses
    -Wlogical-op
    -Wmain
    -Wmaybe-uninitialized
    -Wmemset-transposed-args
    -Wmismatched-new-delete
    -Wmismatched-tags
    -Wmissing-braces
    -Wmissing-declarations
    -Wmissing-field-initializers
    -Wmultichar
    -Wnarrowing
    -Wnoexcept-type
    -Wnon-template-friend
    -Wnonnull
    -Wodr
    -Wopenmp-simd
    -Woverflow
    -Woverlength-strings
    -Woverloaded-virtual
    -Wpacked
    -Wpacked-bitfield-compat
    -Wparentheses
    -Wpessimizing-move
    -Wplacement-new=2
    -Wpmf-conversions
    -Wpointer-arith
    -Wpragmas
    -Wrange-loop-construct
    -Wredundant-move
    -Wregister
    -Wreorder
    -Wreturn-local-addr
    -Wreturn-type
    -Wsequence-point
    -Wshift-count-negative
    -Wshift-count-overflow
    -Wsign-compare
    -Wsign-promo
    -Wsized-deallocation
    -Wsizeof-array-argument
    -Wsizeof-pointer-memaccess
    -Wstack-protector
    -Wstrict-null-sentinel
    -Wsuggest-attribute=const
    -Wsuggest-attribute=noreturn
    -Wsuggest-attribute=pure
    -Wsuggest-final-methods
    -Wsuggest-final-types
    -Wswitch
    -Wswitch-bool
    -Wsync-nand
    -Wsynth
    -Wsystem-headers
    -Wtrampolines
    -Wtrigraphs
    -Wtype-limits
    -Wuninitialized
    -Wunknown-pragmas
    -Wunsafe-loop-optimizations
    -Wunused
    -Wunused-but-set-parameter
    -Wunused-but-set-variable
    -Wunused-function
    -Wunused-label
    -Wunused-local-typedefs
    -Wunused-macros
    -Wunused-parameter
    -Wunused-result
    -Wunused-value
    -Wunused-variable
    -Wvarargs
    -Wvariadic-macros
    -Wvector-operation-performance
    -Wvirtual-move-assign
    -Wvla
    -Wvolatile
    -Wvolatile-register-var
    -Wwrite-strings

    # Settings disabled until debug can determine if wanted
    -Wno-delete-non-virtual-dtor      # Colliders.hpp
    #-Waggregate-return               # Colliders.hpp
    #-Wpedantic                       # Colliders.hpp __int128
    #-Wsuggest-attribute=format       # Colliders.hpp
    #-Wold-style-cast                 # engine.cpp
    #-Wuseless-cast                   # engine.cpp
    #-Wextra-semi                     # Icons.hpp
    #-Wctor-dtor-privacy              # GameObjects.hpp
    #-Wvirtual-inheritance            # GameObjects.hpp
    #-Wdeprecated                     # types.hpp constexpr static data member
    #-Wfloat-equal                    # types.hpp std::_Hash_impl::hash()
    #-Wmultiple-inheritance           # types.hpp
    #-Wnamespaces                     # types.hpp
    #-Wredundant-tags                 # types.hpp
    #-Wtemplates                      # types.hpp
    #-Wnon-virtual-dtor               # windowbar.hpp?
    #-Wswitch-enum                    # Serializer.cpp
    #-Wswitch-default                 # libstdc++ labmda functions
    #-Wcast-qual                      # toml.hpp?
    #-Wconversion                     # toml.hpp?
    #-Wconversion-null                # toml.hpp?
    #-Wdouble-promotion               # toml.hpp?
    #-Weffc++                         # toml.hpp?
    #-Wlong-long                      # toml.hpp?
    #-Wnoexcept                       # toml.hpp?
    #-Wpadded                         # toml.hpp?
    #-Wundef                          # toml.hpp?
    #-Wzero-as-null-pointer-constant  # toml.hpp?
    #-Wredundant-decls                # toml.hpp?
    #-Wshadow                         # toml.hpp?
    #-Wsuggest-override               # toml.hpp?
)

# GCC/CMake C++ Items to explicitly allow
add_compile_options(
    -Wno-missing-include-dirs
)


# Include header files
file(GLOB INCLUDE_SOURCES "include/*.hpp")
include_directories("include")

# Include raylib
include_directories("/home/aether/raylib/cmake")
set(raylib_DIR "/home/aether/raylib/cmake")
find_package(raylib REQUIRED)

# Add source files
file(GLOB SOURCE_FILES "src/*.cpp")

# Add executable
add_executable(Game_Port ${SOURCE_FILES} ${INCLUDE_SOURCES})

# Link libraries
target_link_libraries(Game_Port ${raylib_LIBRARIES})