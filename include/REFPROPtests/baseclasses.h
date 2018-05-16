#ifndef REFPROP_TESTS_BASECLASSES
#define REFPROP_TESTS_BASECLASSES

#include "REFPROP_lib.h"

#include "manyso/native.h"
#include "manyso/exceptions.h"

#include <string>
#include <cstring>

struct REFPROPResult {
    std::vector<double> Output;
    std::string hUnits;
    int iUnit;
    std::vector<double> x, y, x3;
    double q;
    int ierr;
    std::string herr;
};

class REFPROPDLLFixture
{
private:
    std::unique_ptr<NativeSharedLibraryWrapper> RP;
public:
    REFPROPDLLFixture(){ reload(); }

    void reload(){
        char* RPPREFIX = std::getenv("RPPREFIX");
        REQUIRE(RPPREFIX != nullptr);
        REQUIRE(strlen(RPPREFIX) != 0);
#if defined(__MANYSOISWINDOWS__)

#if defined(IS_32_BIT)
        std::string shared_library_filename = "REFPROP.DLL";
#else
        std::string shared_library_filename = "REFPRP64.DLL";
#endif

#else
        std::string shared_library_filename = "librefprop.so";
#endif

        std::string shared_library_path = std::string(RPPREFIX) + shared_library_filename;

#if defined(__MANYSOISWINDOWS__)
        auto load_method = AbstractSharedLibraryWrapper::load_method::FROM_FILE;
#else
        auto load_method = AbstractSharedLibraryWrapper::load_method::LOAD_LIBRARY;
#endif
        RP.reset(new NativeSharedLibraryWrapper(shared_library_path, load_method));
        
        // Check that the load was a success
        bool loaded_properly = std::get<0>(RP->is_locked());
        REQUIRE(loaded_properly);
        
        // Set the path in REFPROP
        SETPATHdll(RPPREFIX, strlen(RPPREFIX));
    }
    virtual ~REFPROPDLLFixture(){
        RP.reset();
    }
    // And now, totally magical, the use of variadic function arguments in concert with type macros
    // Add many methods, each corresponding to a 1-to-1 wrapper of a function from the shared library
    #define X(name) template<class ...Args> void name(Args&&... args){ return RP->getAddress<name ## _POINTER>(#name)(std::forward<Args>(args)...); };
        LIST_OF_REFPROP_FUNCTION_NAMES
    #undef X
    
    int get_enum(const std::string &key) {
        char henum[255], herr[255];
        int ienum = 0, ierr = 0;
        REQUIRE(key.size() < 254);
        strcpy(henum, key.c_str());
        int ii = 0;
        GETENUMdll(ii, henum, ienum, ierr, herr, 255, 255);
        CAPTURE(key);
        CAPTURE(herr);
        REQUIRE(ierr==0);
        return ienum;
    }

    REFPROPResult REFPROP(const std::string &_hFld, const std::string &_hIn, const std::string &_hOut, int unit_system, int iMass, int iFlag, double a, double b, std::vector<double> &z) {
        char hFld[10000], hIn[255], hOut[255];
        REQUIRE(_hFld.size() < 9999);
        REQUIRE(_hIn.size() < 254);
        REQUIRE(_hOut.size() < 254);
        strcpy(hFld, _hFld.c_str());
        strcpy(hIn, _hIn.c_str());
        strcpy(hOut, _hOut.c_str());

        std::vector<double> Output(200), x(20), y(20), x3(20);
        double q;
        int iUnit = 0, ierr = 0;
        char herr[255] = "", hUnits[255] = "";

        REFPROPdll(hFld, hIn, hOut, unit_system, iMass, iFlag, a, b, &(z[0]), &(Output[0]), hUnits, iUnit, &(x[0]), &(y[0]), &(x3[0]), q, ierr, herr, 10000, 255, 255, 255, 255);
        REFPROPResult res  = {Output, hUnits, iUnit, x, y, x3, q, ierr, std::string(herr) };
        return res;
    }
    void FLAGS(const std::string &_hFlag, int jflag, int &kflag){
        char hFlag[255] = "";
        REQUIRE(_hFlag.size() < 254);
        strcpy(hFlag, _hFlag.c_str());

        char herr[255] = "";
        int ierr = 0;
        FLAGSdll(hFlag, jflag, kflag, ierr, herr, 255, 255);
        CAPTURE(herr);
        REQUIRE(ierr == 0);
        if (jflag != -999){
            REQUIRE(kflag == jflag);
        }
    }
};

#endif