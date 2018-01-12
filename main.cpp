#include <cstdlib>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "REFPROPtests/baseclasses.h"

class NBP : public REFPROPDLLFixture
{
public:
    void payload(){
        int ierr = 1, nc = 1;
        char herr[255], hfld[] = "WATER.FLD", hhmx[] = "HMX.BNC", href[] = "DEF";
        SETUPdll(nc,hfld,hhmx,href,ierr,herr,10000,255,3,255);
        CHECK(ierr==0);
        
        int kq = 1;
        double z[] = {1.0}, x[] = {1.0}, y[] = {1.0}, T= 300, p = 101.325, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = 0, w = -1;
        PQFLSHdll(p, q, z, kq, T, d, dl, dv, x, y, u,h,s,cp,cv,w,ierr,herr,255);
        CHECK(ierr == 0);
        CAPTURE(herr);

        CHECK(T == Approx(373.15).margin(0.1));
    }
};

TEST_CASE_METHOD(NBP, "Check NBP of water", "[nbp]")
{
    payload();
};
