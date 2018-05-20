#ifndef REFPROP_TESTS_BASECLASSES
#define REFPROP_TESTS_BASECLASSES

#include "REFPROP_lib.h"

#include "manyso/native.h"
#include "manyso/exceptions.h"
#include "REFPROPtests/utils.h"

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

        std::string shared_library_path = path_join_and_norm(std::string(RPPREFIX), shared_library_filename);
        CAPTURE(shared_library_path);

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
        std::string path(RPPREFIX);
        char hpth[256] = "";
        strcpy(hpth, (path + std::string(255-path.size(), ' ')).c_str());
        SETPATHdll(hpth, 255);
    }
    virtual ~REFPROPDLLFixture(){
        RP.reset(nullptr);
    }
    // And now, totally magical, the use of variadic function arguments in concert with type macros
    // Add many methods, each corresponding to a 1-to-1 wrapper of a function from the shared library
    #define X(name) template<class ...Args> void name(Args&&... args){ return RP->getAddress<name ## _POINTER>(#name)(std::forward<Args>(args)...); };
        LIST_OF_REFPROP_FUNCTION_NAMES
    #undef X
    
    int get_enum(const std::string &key) {
        char henum[256]="", herr[256]="";
        int ienum = 0, ierr = 0;
        REQUIRE(key.size() < 254);
        strcpy(henum, (key + std::string(255-key.size(), ' ')).c_str());
        int ii = 0;
        GETENUMdll(ii, henum, ienum, ierr, herr, 255, 255);
        CAPTURE(key);
        CAPTURE(herr);
        REQUIRE(ierr==0);
        return ienum;
    }

    REFPROPResult REFPROP(const std::string &_hFld, const std::string &_hIn, const std::string &_hOut, int unit_system, int iMass, int iFlag, double a, double b, std::vector<double> &z) {
        char hFld[10001], hIn[256], hOut[256];
        REQUIRE(_hFld.size() < 9999);
        REQUIRE(_hIn.size() < 254);
        REQUIRE(_hOut.size() < 254);
        strcpy(hFld, (_hFld + std::string(10000-_hFld.size(),' ')).c_str());
        strcpy(hIn, (_hIn + std::string(255-_hIn.size(), ' ')).c_str());
        strcpy(hOut, (_hOut + std::string(255-_hOut.size(), ' ')).c_str());

        std::vector<double> Output(200), x(20), y(20), x3(20);
        double q;
        int iUnit = 0, ierr = 0;
        char herr[256] = "", hUnits[256] = "";

        REFPROPdll(hFld, hIn, hOut, unit_system, iMass, iFlag, a, b, &(z[0]), &(Output[0]), hUnits, iUnit, &(x[0]), &(y[0]), &(x3[0]), q, ierr, herr, 10000, 255, 255, 255, 255);
        REFPROPResult res  = {Output, std::string(hUnits), iUnit, x, y, x3, q, ierr, std::string(herr) };
        return res;
    }
    void FLAGS(const std::string &_hFlag, int jflag, int &kflag){
        char hFlag[256] = "";
        REQUIRE(_hFlag.size() <= 255);
        strcpy(hFlag, (_hFlag.c_str() + std::string(255 - _hFlag.size(), ' ')).c_str());

        char herr[256] = "";
        int ierr = 0;
        FLAGSdll(hFlag, jflag, kflag, ierr, herr, 255, 255);
        CAPTURE(herr);
        REQUIRE(ierr == 0);
        if (jflag != -999){
            REQUIRE(kflag == jflag);
        }
    }
    void SETFLUIDS(const std::string &flds, int &ierr, std::string &herr) {
        char hFlds[10001] = "";
        REQUIRE(flds.size() <= 10000);
        strcpy(hFlds, (flds.c_str() + std::string(10000 - flds.size(), ' ')).c_str());
        ierr = 0;
        SETFLUIDSdll(hFlds, ierr, 10000);
        char herrsetup[255] = "";
        if (ierr != 0) {
            ERRMSGdll(ierr, herrsetup, 255);
        }
        herr = std::string(herrsetup);
    }
};

#endif