
#include <cstdlib>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define REFPROP_IMPLEMENTATION
#include "REFPROP_lib.h"
#undef REFPROP_IMPLEMENTATION

class REFPROPDLLFixture
{
public:
    REFPROPDLLFixture(){
        const char* RPPREFIX = std::getenv("RPPREFIX");
        std::cout << RPPREFIX << std::endl;
        std::string shared_library_filename = "hihi";
        std::string err = "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRR";
        bool did_load = load_REFPROP(err, RPPREFIX, shared_library_filename);
        REQUIRE(did_load);
    }
    ~REFPROPDLLFixture(){
        std::string err;
        bool did_unload = unload_REFPROP(err);
        REQUIRE(did_unload);
    }
    virtual void payload() = 0;
};

class NBP : public REFPROPDLLFixture
{
public:
    NBP() : REFPROPDLLFixture() { 
        std::cout << "NBP init\n"; 
    };
    void payload(){
        std::cerr << "payyy\n";
        
        const char* RPPREFIX = std::getenv("RPPREFIX");
        REQUIRE(SETUPdll != nullptr);
        SETPATHdll(RPREFIX, strlen(RPPREFIX));

        long ierr = 1, nc = 1;
        char herr[255], hfld[] = "WATER.FLD", hhmx[] = "HMX.BNC", href[] = "DEF";
        SETUPdll(nc,hfld,hhmx,href,ierr,herr,10000,255,3,255);
        CHECK(ierr==0);
        
        /*if (ierr > 0) printf("This ierr: %ld herr: %s\n", ierr, herr);
        {
            long ierr = 0;
            char herr[255];
            double z[] = {1.0}, x[] = {1.0}, y[] = {1.0}, T= 300, p = 101.325, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = -1, w = -1;
            TPFLSHdll(T, p, z, d, dl, dv, x, y, h,s,u,cp,cv,w,q,ierr,herr,255);
            if (ierr > 0) printf("This ierr: %ld herr: %s\n", ierr, herr);
            printf("This d: %g mol/L\n", d);
        }
        */
    }
};

TEST_CASE_METHOD(NBP, "Check NBP of water", "[nbp]")
{
    payload();
};
