#ifndef REFPROP_TESTS_BASECLASSES
#define REFPROP_TESTS_BASECLASSES

#include "manyso/windows/windows.h"
#include "manyso/exceptions.h"

#include "REFPROP_lib.h"

class REFPROPDLLFixture
{
private:
    std::unique_ptr<WindowsSharedLibraryWrapper> RP;
public:
    REFPROPDLLFixture(){
        char* RPPREFIX = std::getenv("RPPREFIX");
        REQUIRE(RPPREFIX != nullptr);
        REQUIRE(strlen(RPPREFIX) != 0);
        std::string shared_library_filename = "REFPRP64.DLL";
        std::string shared_library_path = std::string(RPPREFIX) + shared_library_filename;

        auto load_method = AbstractSharedLibraryWrapper::load_method::FROM_FILE;
        RP.reset(new WindowsSharedLibraryWrapper(shared_library_path, load_method));
        
        // Check that the load was a success
        bool loaded_properly = std::get<0>(RP->is_locked());
        REQUIRE(loaded_properly);
        
        // Set the path in REFPROP
        SETPATHdll(RPPREFIX, strlen(RPPREFIX));
    }
    ~REFPROPDLLFixture(){
        RP.release();
    }
    // And now, totally magical, the use of variadic function arguments in concert with type macros
    // Add many methods, each corresponding to a 1-to-1 wrapper of a function from the shared library
    #define X(name) template<class ...Args> void name(Args&&... args){ return RP->getAddress<name ## _POINTER>(#name)(std::forward<Args>(args)...); };
        LIST_OF_REFPROP_FUNCTION_NAMES
    #undef X

    virtual void payload() = 0;
};

#endif