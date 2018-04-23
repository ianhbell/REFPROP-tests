#ifndef REFPROP_TESTS_BASECLASSES
#define REFPROP_TESTS_BASECLASSES

#include "REFPROP_lib.h"

#include "manyso/native.h"
#include "manyso/exceptions.h"

#include <string>
#include <cstring>

class REFPROPDLLFixture
{
private:
    std::unique_ptr<NativeSharedLibraryWrapper> RP;
public:
    REFPROPDLLFixture(){
        char* RPPREFIX = std::getenv("RPPREFIX");
        REQUIRE(RPPREFIX != nullptr);
        REQUIRE(strlen(RPPREFIX) != 0);
        std::string shared_library_filename = "REFPRP64.DLL";
        std::string shared_library_path = std::string(RPPREFIX) + shared_library_filename;

        auto load_method = AbstractSharedLibraryWrapper::load_method::FROM_FILE;
        RP.reset(new NativeSharedLibraryWrapper(shared_library_path, load_method));
        
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
    
    int get_enum(const std::string &key) {
        char henum[255], herr[255];
        int ienum = 0, ierr = 0;
        strcpy(henum, key.c_str());
        int ii = 0;
        GETENUMdll(ii, henum, ienum, ierr, herr, 255, 255);
        CAPTURE(key);
        CAPTURE(herr);
        REQUIRE(ierr==0);
        return ienum;
    }
    
    virtual void payload() = 0;
};

#endif