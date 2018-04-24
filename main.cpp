#include <cstdlib>
#include <boost/algorithm/string/trim.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "REFPROPtests/baseclasses.h"
#include "REFPROPtests/utils.h"

class NBP : public REFPROPDLLFixture
{
public:
    void payload(){
        int ierr = 1, nc = 1;
        char herr[255], hfld[255] = "WATER.FLD", hhmx[255] = "HMX.BNC", href[4] = "DEF";
        SETUPdll(nc,hfld,hhmx,href,ierr,herr,10000,255,3,255);
        CHECK(ierr==0);
        
        int kq = 1;
        double z[] = {1.0}, x[] = {1.0}, y[] = {1.0}, T= 300, p = 101.325, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = 0, w = -1;
        PQFLSHdll(p, q, z, kq, T, d, dl, dv, x, y, u,h,s,cp,cv,w,ierr,herr,255);
        CAPTURE(herr); 
        CHECK(ierr == 0);

        CHECK(T == Approx(373.15).margin(0.1));
    }
};
TEST_CASE_METHOD(NBP, "Check NBP of water", "[nbp]"){ payload(); };

class WeirdFiles : public REFPROPDLLFixture
{
public:
    void payload() {
        int ierr = 1, nc = 1;
        char herr[255], hfld[] = "-10.0.FLD", hhmx[] = "HMX.BNC", href[] = "DEF";
        SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
        CAPTURE(herr);
        REQUIRE(ierr == 0);
    }
};
TEST_CASE_METHOD(WeirdFiles, "Check fluid files with dash in them", "[file_loading]") { payload(); };

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
TEST_CASE_METHOD(mixNBP, "Check TP flash of multicomponent mixture", "[flash]"){ payload(); };

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

            double T, p=101.325, D, Dl, Dv, xliq[20], xvap[20], q=0, u,h,s,cv,cp,w;
            ierr=0; char herr[255]="";

            // Calculate T_nbp given p_nbp (101.325 kPa)
            PQFLSHdll(p, q, z, kq, T, D, Dl, Dv, xliq, xvap, u,h,s,cv,cp,w,ierr, herr, 255);
            CAPTURE(fld);
            CAPTURE(herr); 
            CHECK(ierr == 0);
            CHECK(T == Approx(tnbpt).margin(0.1));

            // Now do the inverse calculation T_nbp->p_nbp
            T = tnbpt;
            TQFLSHdll(T, q, z, kq, p, D, Dl, Dv, xliq, xvap, u, h, s, cv, cp, w, ierr, herr, 255);
            CAPTURE(fld);
            CAPTURE(herr); 
            CHECK(ierr == 0);
            CHECK(p == Approx(101.325).margin(0.1));
            
        }
    }
};
TEST_CASE_METHOD(pureNBP, "Check NBP for all pure fluids (when possible)", "[flash]"){ payload(); };

class pureTrip : public REFPROPDLLFixture
{
public:
    void payload() {
        for (auto &fld : get_pure_fluids_list()) {
            int ierr = 0; char cfld[10000];
            strcpy(cfld, fld.c_str());
            SETFLUIDSdll(cfld, ierr, 255);
            REQUIRE(ierr == 0);

            double wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas, z[] = { 1.0 };
            int icomp = 1, kq = 1;
            INFOdll(icomp, wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas);

            double p = 101.325, D, Dl, Dv, xliq[20], xvap[20], q = 0, u, h, s, cv, cp, w;
            ierr = 0; char herr[255] = "";

            char htyp[4] = "EOS"; double Tmin, Tmax, Dmax, Pmax;
            LIMITSdll(htyp, z, Tmin, Tmax, Dmax, Pmax, 3);
            if (Tmin > ttrp) {
                // Skip fluids where Tmin < Trip
                continue;
            }

            // Now do the calculation at T_trp
            TQFLSHdll(ttrp, q, z, kq, p, D, Dl, Dv, xliq, xvap, u, h, s, cv, cp, w, ierr, herr, 255);
            CAPTURE(fld);
            CAPTURE(herr);
            CHECK(ierr == 0);
        }
    }
};
TEST_CASE_METHOD(pureTrip, "Check Ttriple for all pure fluids (when possible)", "[flash]"){ payload(); };

class AbsPathSETUP : public REFPROPDLLFixture
{
public:
    void payload() {
        std::string fld = std::string(std::getenv("RPPREFIX")) + "/FLUIDS/ACETYLENE.FLD";
        int ierr = 0; char cfld[10000];
        strcpy(cfld, fld.c_str());
        SETFLUIDSdll(cfld, ierr, 255);
        if (ierr != 0) {
            int _ierr;
            char herr[255];
            ERRMSGdll(_ierr, herr, 255);
            CAPTURE(herr);
        }
        REQUIRE(ierr == 0);
        double wmol=0; int i = 1;
        WMOLIdll(i, wmol);
        CHECK(wmol > 26);
        CHECK(wmol < 27);
    }
};
TEST_CASE_METHOD(AbsPathSETUP, "Check absolute paths are ok", "[setup]") { payload(); };

class FullAbsPathSETUP : public REFPROPDLLFixture
{
public:

    void payload() {
        int MOLAR_BASE_SI = get_enum("MOLAR BASE SI");

        std::string hmx = std::string(std::getenv("RPPREFIX")) + "/FLUIDS/HMX.BNC";
       // hmx += std::string(255-hmx.size(), '\0');
        char * hhmx = const_cast<char *>(hmx.c_str());
        
        for (char sep : {'*',';'})
        {
            std::string fld0 = std::string(std::getenv("RPPREFIX")) + "FLUIDS/R32.FLD";
            std::string fld1 = std::string(std::getenv("RPPREFIX")) + "FLUIDS/PROPANE.FLD";
            std::string fld = fld0 + std::string(1, sep) + fld1;
            std::string flds = fld + std::string(10000-fld.size(), '\0');
            char hfld[10000];
            strcpy(hfld, flds.c_str());
            char hin[255] = "", hout[255] = "FDIR(1)", hUnits[255], herr[255];
            int iMass = 0, iFlag = 0, iUnit = 0, ierr = 0;
            double Output[200], x[20], y[20], x3[20], z[20] = { 0.5,0.5 }, q, a = 1, Q = 0.0;
            REFPROPdll(hfld, hin, hout, MOLAR_BASE_SI, iMass, iFlag, a, Q, z, Output, hUnits, iUnit, x, y, x3, q, ierr, herr, 10000U, 255U, 255U, 255U, 255U);
            CAPTURE(hUnits);
            CAPTURE(sep);
            CAPTURE(herr);
            std::string str = std::string(herr);
            boost::algorithm::trim(str);
            CHECK(str == fld0);
            CHECK(ierr == -999);
        }
        {
            std::string flds = std::string(std::getenv("RPPREFIX")) + "FLUIDS/R32.FLD" + "|" + std::string(std::getenv("RPPREFIX")) + "FLUIDS/PROPANE.FLD";
            flds += std::string(10000-flds.size(), '\0');
            char * hfld = const_cast<char *>(flds.c_str());
            int ierr = 0, nc=2; char hdef[4] = "DEF", herr[255];
            SETUPdll(nc,hfld, hhmx, hdef, ierr, herr, 10000,255,3,255);
            CAPTURE(herr);
            CHECK(ierr == 0);
            double wmol = 0; int i = 1;
            WMOLIdll(i, wmol);
            CHECK(wmol > 50);
            CHECK(wmol < 55);
        }
        {
            std::string flds = "R32;PROPANE" + std::string(5000, '\0');
            char * hfld = const_cast<char *>(flds.c_str());
            char hin[255] = "TQ", hout[255] = "D", hUnits[255], herr[255];
            int iMass = 0, iFlag = 0, iUnit, ierr = 0;
            double Output[200], x[20], y[20], x3[20], z[2] = { 0.5,0.5 }, q, T = 300.0, Q = 0.0;
            REFPROPdll(hfld, hin, hout, MOLAR_BASE_SI, iMass, iFlag, T, Q, z, Output, hUnits, iUnit, x, y, x3, q, ierr, herr, 10000, 255, 255, 255, 255);
            CAPTURE(herr);
            CHECK(ierr == 0);
        }
    }
};
TEST_CASE_METHOD(FullAbsPathSETUP, "Check full absolute paths are ok", "[setup]") { payload(); };

class CheckZEZEstimated : public REFPROPDLLFixture
{
public:
    void payload() {
        int MOLAR_BASE_SI = get_enum("MOLAR BASE SI");
        std::string flds = "R134A*R1234ZEZ" + std::string(500, '\0');
        char * hfld = const_cast<char *>(flds.c_str());
        char hin[255] = " ", hout[255] = "FIJMIX", hUnits[255], herr[255];
        int iMass = 0, iFlag = 0, iUnit, ierr = 0;
        double Output[200], x[20], y[20], x3[20], z[2] = { 0.5,0.5 }, q, a = 1, b = 2;
        REFPROPdll(hfld, hin, hout, MOLAR_BASE_SI, iMass, iFlag, a, b, z, Output, hUnits, iUnit, x, y, x3, q, ierr, herr, 10000, 255, 255, 255, 255);
        CAPTURE(herr);
        CHECK(ierr == -117);
    }
};
TEST_CASE_METHOD(CheckZEZEstimated, "CheckZEZEstimated", "[setup]") { payload(); };

class CheckZEFails : public REFPROPDLLFixture
{
public:
    void payload() {
        char cfld[255] = "R1234ze";
        int ierr = 0;
        SETFLUIDSdll(cfld, ierr, 255);
        if (ierr != 0) {
            char herr[255];
            ERRMSGdll(ierr, herr, 255);
            CAPTURE(herr);
        }
        CHECK(ierr == 851);
    }
};
TEST_CASE_METHOD(CheckZEFails, "Check that R1234ze is an invalid fluid name", "[setup]") { payload(); };

class CheckAllLoads : public REFPROPDLLFixture
{
public:
    void payload() {
        for (auto &fld : get_pure_fluids_list()) {
            int ierr = 0; char cfld[10000];
            strcpy(cfld, fld.c_str());
            SETFLUIDSdll(cfld, ierr, 255);
            if (ierr != 0) {
                char herr[255];
                ERRMSGdll(ierr, herr, 255);
                CAPTURE(herr);
            }
            CHECK(ierr == 0);
        }
    }
};
TEST_CASE_METHOD(CheckAllLoads, "Check that all fluids load properly", "[setup]") { payload(); };

class SUBTVALIDATION : public REFPROPDLLFixture
{
public:
    struct PsubCheck {
        std::string name;
        double T_K, p_Pa, eps_tol;
    };
    std::vector<PsubCheck> points = {
        {"SF6", 209.7416748, 0.101325e6, 0.001}, // Guder and Span
        {"H2S", 164.932, 0.0031937e6,0.001},
        {"AMMONIA", 176.957, 8.0746e2, 0.01},
        {"CO",61.55, 3.7596e3, 0.00001},
        {"ETHYLENE",90, 3.5004, 0.0001},
        {"ETHANE", 80.808, 4.2065E-2, 0.0001},
        {"FLUORINE", 50, 6.75998e2,0.001},
        {"N2O", 153.84615384615384, 0.00474895372822e6, 0.001 },
        {"PARAHYD", 10, 2.6206e2, 0.001},
        {"HYDROGEN", 8.76, 44.4713821784, 0.001 },
        {"NEON", 20, 3.81191E3,0.001 }
    };
    void payload() {
        for (auto &&pt : points){
            std::string fld = pt.name;
            int ierr = 0; char cfld[10000];
            strcpy(cfld, fld.c_str());
            SETFLUIDSdll(cfld, ierr, 255);
            if (ierr != 0) {
                int _ierr;
                char herr[255];
                ERRMSGdll(_ierr, herr, 255);
                CAPTURE(herr);
            }
            REQUIRE(ierr == 0);

            double x[1] = {1.0};
            double p_kPa;
            char herr[255];
            SUBLTdll(pt.T_K, x, p_kPa, ierr,herr,255);
            double p_Pa = p_kPa*1000;
            CAPTURE(herr);
            REQUIRE(ierr == 0);
            CHECK(p_Pa == Approx(pt.p_Pa).epsilon(pt.eps_tol));
        }
    }
};
TEST_CASE_METHOD(SUBTVALIDATION, "Check sublimation pressures", "[sublimation]") { payload(); };