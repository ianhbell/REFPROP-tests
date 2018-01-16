#include <cstdlib>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "REFPROPtests/baseclasses.h"
#include "REFPROPtests/utils.h"


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

class mixNBP : public REFPROPDLLFixture
{
public:
    void payload() {
        char hfld[10000] = "NITROGEN|WATER|CO2|H2S|METHANE|ETHANE|PROPANE|ISOBUTAN|BUTANE|IPENTANE|PENTANE";
        double z[] = {1.2000036000108E-03,7.000021000063E-06,.828792486377459,2.000006000018E-04,.160400481201444,7.6000228000684E-03,1.4000042000126E-03,1.000003000009E-04,2.000006000018E-04,0,1.000003000009E-04};
        int ierr = 0, nc = 11;
        char herr[255], hhmx[] = "HMX.BNC", href[] = "DEF";
        SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
        REQUIRE(ierr == 0);

        ierr = 0;
        double x[30] = { 1.0 }, y[20] = { 1.0 }, T = 313.15, p = 400, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = 0, w = -1;
        TPFLSHdll(T, p, z, d, dl, dv, x, y, q, u, h, s, cp, cv, w, ierr, herr, 255);
        CAPTURE(herr); 
        REQUIRE(ierr == 0);
    }
};

TEST_CASE_METHOD(mixNBP, "Check TP flash of multicomponent mixture", "[flash]")
{
    payload();
};

class pureNBP : public REFPROPDLLFixture
{
public:
    void payload() {
        for (auto &fld : get_pure_fluids_list()) {
            int ierr = 0;
            char cfld[10000]; 
            strcpy(cfld, fld.c_str());
            SETFLUIDSdll(cfld, ierr, 255);
            REQUIRE(ierr == 0); 
            
            double wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas, z[] = {1.0};
            int icomp = 1, kq = 1;
            INFOdll(icomp, wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas);

            double T, p=101.325, x[20], D, Dl, Dv, xliq[20], xvap[20], q=0, u,h,s,cv,cp,w;
            ierr=0; char herr[255]="";
            // Calculate p_nbp given T_nbp
            PQFLSHdll(p, q, z, kq, T, D, Dl, Dv, xliq, xvap, u,h,s,cv,cp,w,ierr, herr, 255);
            CAPTURE(fld);
            CHECK(ierr == 0);
            CHECK(T == Approx(tnbpt).margin(0.1));
            CAPTURE(herr);

            // Now do the inverse calculation T_nbp->p_nbp
            T = tnbpt;
            TQFLSHdll(T, q, z, kq, p, D, Dl, Dv, xliq, xvap, u, h, s, cv, cp, w, ierr, herr, 255);
            CAPTURE(fld);
            CHECK(ierr == 0);
            CHECK(p == Approx(101.325).margin(0.1));
            CAPTURE(herr);

        }
    }
};

TEST_CASE_METHOD(pureNBP, "Check NBP for all pure fluids (when possible)", "[flash]")
{
    payload();
};