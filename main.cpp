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

class EnablePR : public REFPROPDLLFixture
{
public:
    void payload() {

        // SETUP
        int ierr = 0, nc =2;
        char hfld[10000] = "METHANE|ETHANE";
        char herr[255], hhmx[] = "HMX.BNC", href[] = "DEF";
        SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
        REQUIRE(ierr == 0);

        // Turn on PR
        int prflag = 2;
        PREOSdll(prflag);
        
        // Get the parameters
        int icomp = 1, jcomp = 2;
        ierr = 0;
        char hmodij[3], hfmix[255], hbinp[255], hfij[255], hmxrul[255];
        double fij[6];
        GETKTVdll(icomp, jcomp, hmodij, fij, hfmix, hfij, hbinp, hmxrul, 3, 255, 255, 255, 255);
        std::string mod = std::string(hmodij, 3);
        CAPTURE(mod);
        REQUIRE(mod == "PR ");
    }
};
TEST_CASE_METHOD(EnablePR, "EnablePR", "[setup]") { payload(); };

class pureNBP : public REFPROPDLLFixture
{
public:
    void payload() {
        for (auto &fld : get_pure_fluids_list()) {
            {
                int ierr = 0;
                char cfld[10000]; 
                strcpy(cfld, fld.c_str());
                SETFLUIDSdll(cfld, ierr, 255);
                char herrsetup[255] = "";
                if (ierr != 0) {
                    ERRMSGdll(ierr, herrsetup, 255);
                }
                CAPTURE(herrsetup);
                CAPTURE(cfld);
                REQUIRE(ierr == 0); 
            }

            int MOLAR_BASE_SI = get_enum("MOLAR BASE SI");
            {
                int iMass = 0, iFlag = 0;
                std::vector<double> z = { 0.5,0.5 }; double a = 1, Q = 0.0;
                auto r = REFPROP(fld, "TRIP", "P", MOLAR_BASE_SI, iMass, iFlag, a, Q, z);
                double pt = r.Output[0];
                CAPTURE(r.herr);
                CHECK(r.ierr < 100);
                if (pt > 101325) {
                    // Impossible; skip this fluid
                    continue;
                }
            }
            
            double wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas, z[] = {1.0};
            int icomp = 1, kq = 1;
            INFOdll(icomp, wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas);

            double T, p=101.325, D, Dl, Dv, xliq[20], xvap[20], q=0, u,h,s,cv,cp,w;
            int ierr=0; char herr[255]="";
            
            char htyp[4] = "EOS"; double Tmin, Tmax, Dmax, Pmax;
            LIMITSdll(htyp, z, Tmin, Tmax, Dmax, Pmax, 3);

            // Calculate T_nbp given p_nbp (101.325 kPa)
            PQFLSHdll(p, q, z, kq, T, D, Dl, Dv, xliq, xvap, u,h,s,cv,cp,w,ierr, herr, 255);
            CAPTURE(fld);
            CAPTURE(herr); 
            CHECK(ierr < 100);
            if (T < Tmin){ continue; }
            CHECK(T == Approx(tnbpt).margin(0.01));

            // Now do the inverse calculation T_nbp->p_nbp
            T = tnbpt; ierr = 0;
            TQFLSHdll(T, q, z, kq, p, D, Dl, Dv, xliq, xvap, u, h, s, cv, cp, w, ierr, herr, 255);
            CAPTURE(fld);
            CAPTURE(herr); 
            CHECK(ierr < 100);
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
            char herrsetup[255] = "";
            if (ierr != 0) {
                ERRMSGdll(ierr, herrsetup, 255);
            }
            CAPTURE(herrsetup);
            CHECK(ierr == 0);

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
        char herr[255];
        if (ierr != 0) {
            ERRMSGdll(ierr, herr, 255);
        }
        CAPTURE(herr);
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

struct KTVvalues {
    std::string hmodij; 
    std::vector<double> fij;
    std::string hfmix, hfij, hbinp, hmxrul;
};
class GETSETKTV : public REFPROPDLLFixture
{
private:
    enum class perturbations { reset, peng_robinson, AGA, set_back, jiggle_then_reset };

public:
    KTVvalues get_values(int icomp = 1, int jcomp = 2) {
        char hmodij[3], hfmix[255], hfij[255], hbinp[255], hmxrul[255];
        double fij[6];
        GETKTVdll(icomp, jcomp, hmodij, fij, hfmix, hfij, hbinp, hmxrul, 3, 255, 255, 255, 255);
        std::vector<double> v(fij,fij+6);
        KTVvalues o{std::string(hmodij,3), v, std::string(hfmix,254), std::string(hfij,254), std::string(hbinp,254), std::string(hmxrul,254)};
        boost::algorithm::trim(o.hmodij); //inplace
        boost::algorithm::trim(o.hfmix); //inplace
        boost::algorithm::trim(o.hfij); //inplace
        boost::algorithm::trim(o.hbinp); //inplace
        boost::algorithm::trim(o.hmxrul); //inplace
        return o;
    }
    void set_values(const KTVvalues in, int icomp = 1, int jcomp = 2) {
        char hmodij[4] = "", hfmix[255], hfij[255], hbinp[255], hmxrul[255];
        strcpy(hmodij, in.hmodij.c_str());
        strcpy(hfmix, in.hfmix.c_str());
        strcpy(hfij, in.hfij.c_str());
        strcpy(hbinp, in.hbinp.c_str());
        strcpy(hmxrul, in.hmxrul.c_str());
        double fij[6]; for (auto i = 0; i < in.fij.size(); ++i){ fij[i] = in.fij[i]; }
        int ierr = 0; char herr[255];
        SETKTVdll(icomp, jcomp, hmodij, fij, hfmix, ierr, herr, 3, 255, 255);
        CAPTURE(herr);
        CHECK(ierr == 0);
    }
    KTVvalues jiggle(const KTVvalues in) {
        KTVvalues out = in;
        out.fij[0] += 0.0123;
        return out;
    }
    void reset() {
        int icomp = 1, jcomp = 2;
        char hmodij[4] = "RST", hfmix[255]; double fij[6]; 
        int ierr = 0; char herr[255];
        SETKTVdll(icomp, jcomp, hmodij, fij, hfmix, ierr, herr, 3, 255, 255);
        CAPTURE(herr);
        CHECK(ierr == 0);
    }
    void do_action(perturbations step) {
        switch (step) {
            case perturbations::peng_robinson:
            {
                // Initial values
                auto init = get_values();
                // Turn on PR
                int prflag = 2;
                PREOSdll(prflag);
                auto pr_init = get_values();
                // Check PR is on
                CHECK(pr_init.hmodij ==
                    std::string("PR"));
                // Jiggle kij parameter
                auto jig = jiggle(pr_init);
                set_values(jig);
                auto nv = get_values();
                // Check PR is on, and parameters are modified
                CHECK(nv.hmodij == std::string("PR"));
                CHECK(nv.hbinp != pr_init.hbinp);
                // Turn off PR
                prflag = 0;
                PREOSdll(prflag);
                auto nv2 = get_values();
                // Check PR is still on, but that parameters have been set back to initial parameters
                CHECK(nv2.hmodij == std::string("PR"));
                CHECK(are_same(get_values(), pr_init));
                // Reset the parameters
                reset();
                CHECK(are_same(get_values(), init));
            }
            case perturbations::AGA:
            {
                auto init = get_values();
                int ierr; char herr[255];
                SETAGAdll(ierr, herr, 255);
                auto flags = get_values();
                // Check same as before
                CHECK(are_same(init, flags));
                // Turn off AGA
                UNSETAGAdll();
            }
            case perturbations::jiggle_then_reset:
            {
                auto init = get_values();
                auto jig = jiggle(init);
                set_values(jig);
                auto flags = get_values();
                // Check same as before
                CHECK(are_same(jig, flags));
                // Reset
                reset();
                // Check same as beginning
                CHECK(are_same(init, get_values()));
            }
            case perturbations::set_back:
            {
                auto init = get_values();
                set_values(init);
                auto flags = get_values();
                // Check same as before
                CHECK(are_same(init, flags));
                // Reset
                reset();
                // Check same as beginning
                CHECK(are_same(init, get_values()));
            }
        }
    }
    bool are_same(KTVvalues in1, KTVvalues in2) {
        if (in1.fij.size() != in2.fij.size()) { return false; }
        for (auto i = 0; i < in1.fij.size(); ++i) {
            if (std::abs(in1.fij[i] - in2.fij[i]) > 1e-12) {
                return false;
            }
        }
        if (in1.hmodij != in2.hmodij) { return false; }
        if (in1.hfij != in2.hfij) { return false; }
        if (in1.hmxrul != in2.hmxrul) { return false; }
        return true;
    }
    void payload() {
        auto binary_pairs = get_binary_pairs();
        CHECK(binary_pairs.size() > 0);

        for (const std::string & fluids: {"Methane * Ethane", "R1234yf * R134a" }){
            char cfld[255] = "";
            CAPTURE(fluids);
            strcpy(cfld, fluids.c_str());
            int ierr = 0;
            SETFLUIDSdll(cfld, ierr, 255);
            char herr[255] = "";
            if (ierr != 0) {
                ERRMSGdll(ierr, herr, 255);
            }
            CAPTURE(herr);
            // Get the initial state
            auto ktv = get_values();

            // Run each step, checking the output each time
            std::vector<perturbations> steps = {
                perturbations::AGA, 
                perturbations::peng_robinson, 
                perturbations::peng_robinson,
                perturbations::set_back
            };
            for (auto &step : steps) {
                do_action(step);
            }
            
            // Do we end where we started?
            auto new_ktv = get_values();
            CHECK(are_same(ktv, new_ktv));
        }
    }
};
TEST_CASE_METHOD(GETSETKTV, "Get and set B.I.P.", "[BIP]") { payload(); };


class CRITP : public REFPROPDLLFixture
{
public:
    void payload() {
        char cfld[255] = "Methane * Ethane";
        int ierr = 0;
        SETFLUIDSdll(cfld, ierr, 255);
        char herr[255];
        if (ierr != 0) {
            ERRMSGdll(ierr, herr, 255);
        }
        CAPTURE(herr);

        double z[20] = {0.5,0.5};
        ierr = 0;
        char herr2[255] = "";
        double Tcrit, pcrit_kPa, dcrit_mol_L;
        CRITPdll(z, Tcrit, pcrit_kPa, dcrit_mol_L, ierr, herr2, 255);
        CAPTURE(herr2);
        CHECK(ierr <= 0);
    }
};
TEST_CASE_METHOD(CRITP, "Critical TC1, VC1", "[crit]") { payload(); };

class CASPROPANE : public REFPROPDLLFixture
{
public:
    void payload() {
        std::vector<double> z = {1.0};
        auto r = REFPROP("PROPANE", " ", "CAS#", 0,0,0,0,0,z);
        CAPTURE(r.herr);
        CHECK(r.ierr == 0);
        boost::algorithm::trim(r.hUnits); //inplace
        CHECK(r.hUnits == "74-98-6");
    }
};
TEST_CASE_METHOD(CASPROPANE, "CAS# for PROPANE", "[CAS]") { payload(); };

class Torture : public REFPROPDLLFixture
{
public:
    void payload() {
        {
            char hflag[255] = "Debug", herr[255] = "";
            int jflag = 1, kflag = -1, ierr = 0;
            FLAGSdll(hflag, jflag, kflag, ierr, herr, 255,255);
        }
        for (auto &&pair : get_binary_pairs()){
            for (std::string &&k: {"ETA", "TCX", "CP", "P", "STN", "TC", "PC"}){
                for (bool forwards : {true, false}){
                    std::vector<double> z;
                    if (forwards) {
                        z = {0.4,0.6};
                    }
                    else {
                        z = {0.6,0.4};
                    }
                    std::string fluids = (forwards) ? pair.first + "*" + pair.second : pair.second + "*" + pair.first;
                    REFPROP(fluids, "PT", k, 1, 0, 0, 0.101325, 300, z);
                }
            }
        }
    }
};
TEST_CASE_METHOD(Torture, "Torture test calling of DLL", "[Torture]") { payload(); };

class AlpharDerivs : public REFPROPDLLFixture
{
public:
    void payload() {
        for (auto &&pair : get_binary_pairs()) {
            // Setup the fluids
            int ierr = 0;
            char flds[10000];
            std::string joined = pair.first + ";" + pair.second;
            strcpy(flds, joined.c_str());
            SETFLUIDSdll(flds, ierr, 255);
            char herr[255] = "";
            if (ierr != 0) {
                ERRMSGdll(ierr, herr, 255);
            }
            CAPTURE(herr);
            if (ierr == 101) {
                continue;
            }
            CHECK(ierr == 0);

            double tau  = 0.9, delta = 1.01;
            std::vector<double> z = {0.0, 1.0};
            auto deriv = [this, &z](int itau, int idelta, double tau, double delta) {
                double arderiv;
                PHIXdll(itau, idelta, tau, delta, &(z[0]), arderiv);
                return arderiv/(pow(tau ,itau)*pow(delta, idelta));
            };
            for (int itau = 0; itau <= 3; ++itau) {
                for (int idelta = 0; idelta <= 3; ++idelta) {
                    if (itau + idelta > 4 || itau + idelta == 0) {
                        continue;
                    }
                    // centered derivative based on the lower-order derivative
                    double numeric;
                    if (itau > 0){
                        double dtau = 1e-5;
                        numeric = (-deriv(itau-1, idelta, tau+2*dtau, delta)
                            + 8 * deriv(itau - 1, idelta, tau+dtau, delta)
                            - 8 * deriv(itau - 1, idelta,  tau-dtau, delta)
                            + deriv(itau - 1, idelta, tau-2*dtau, delta)) / (12 * dtau);
                    }
                    else {
                        double ddelta = 1e-5;
                        numeric = (-deriv(itau, idelta-1, tau, delta + 2*ddelta)
                            + 8 * deriv(itau, idelta-1, tau, delta + ddelta)
                            - 8 * deriv(itau, idelta-1, tau, delta - ddelta)
                            + deriv(itau, idelta-1, tau, delta - 2*ddelta)) / (12 * ddelta);
                    }
                    double analytic = deriv(itau, idelta, tau, delta);
                    CAPTURE(itau);
                    CAPTURE(idelta);
                    CAPTURE(joined);
                    CHECK(analytic == Approx(numeric).epsilon(1e-1));
                }
            }
        }
    }
};
TEST_CASE_METHOD(AlpharDerivs, "Check all alphar derivatives", "[alphar]") { payload(); };

/// From Wagner and Pruss, JPCRD, 2002
class WaterAlpharDerivs : public REFPROPDLLFixture
{
public:
    double deriv(int itau, int idelta, double tau, double delta) {
        double arderiv;
        std::vector<double> z = { 1.0 };
        PHIXdll(itau, idelta, tau, delta, &(z[0]), arderiv);
        return arderiv / (pow(tau, itau)*pow(delta, idelta));
    };
    void lowT() {
        double delta = 838.025/322, tau = 647.096/500;
        CHECK(deriv(0,0,tau,delta) == Approx(-0.342693206e1).epsilon(1e-8));
        CHECK(deriv(0,1,tau,delta) == Approx(-0.364366650).epsilon(1e-8));
        CHECK(deriv(0,2,tau,delta) == Approx(0.856063701).epsilon(1e-8));
        CHECK(deriv(1,0,tau,delta) == Approx(-0.581403435e1).epsilon(1e-8));
        CHECK(deriv(2,0,tau,delta) == Approx(-0.223440737e1).epsilon(1e-8));
        CHECK(deriv(1,1,tau,delta) == Approx(-0.112176915e1).epsilon(1e-8));
    }
    void highT() {
        double delta = 358.0/322, tau = 647.096/647;
        CHECK(deriv(0, 0, tau, delta) == Approx(-0.121202657e1).epsilon(1e-8));
        CHECK(deriv(0, 1, tau, delta) == Approx(-0.714012024).epsilon(1e-8));
        CHECK(deriv(0, 2, tau, delta) == Approx(0.475730696).epsilon(1e-8));
        CHECK(deriv(1, 0, tau, delta) == Approx(-0.321722501e1).epsilon(1e-8));
        CHECK(deriv(2, 0, tau, delta) == Approx(-0.996029507e1).epsilon(1e-8));
        CHECK(deriv(1, 1, tau, delta) == Approx(-0.133214720e1).epsilon(1e-8));
    }
    
    void payload() {

        // Setup the fluids
        int ierr = 0;
        char flds[10000] = "Water";
        SETFLUIDSdll(flds, ierr, 255);
        char herr[255] = "";
        if (ierr != 0) {
            ERRMSGdll(ierr, herr, 255);
        }
        CAPTURE(herr);
        CHECK(ierr == 0);
        lowT();
        highT();
    }
};
TEST_CASE_METHOD(WaterAlpharDerivs, "Water alphar derivatives from IAPWS95", "[water],[alphar]") { payload(); };

class StatelessVars : public REFPROPDLLFixture
{
public:
    void payload() {
        // These need splines on...
        //std::vector<std::string> variable_names = { "TMAXT","PMAXT","DMAXT","TMAXP","PMAXP","DMAXP"};
        
        std::vector<std::string> variable_names = { "TC","PC","DC","TCEST","PCEST","DCEST","TRED","DRED","TMIN","TMAX","DMAX","PMAX" };
        std::string joined = variable_names[0];
        for (auto i = 1; i < variable_names.size(); ++i) {
            joined += ";" + variable_names[i];
        }
        for (auto &&pair: get_binary_pairs()){
            auto get_Tred = [this, &pair, &joined, &variable_names](bool forwards) {
                auto flds = (forwards) ? pair.first + "*" + pair.second : pair.second + "*" + pair.first;
                std::vector<double> z;
                if (forwards){
                    z = { 0.4, 0.6 };
                }else{
                    z = { 0.6, 0.4 };
                };
                auto Nvars = variable_names.size();
                auto r = REFPROP(flds, " ", joined, 0, 0, 0, 0, 0, z); 
                CAPTURE(r.herr);
                CHECK(r.ierr < 100);
                return std::vector<double>(r.Output.begin(), r.Output.begin() + Nvars);
            };
            auto forwards = get_Tred(true), back = get_Tred(false), diff = forwards;
            for(auto i = 0; i < forwards.size(); ++i){ 
                diff[i] = std::abs(forwards[i] - back[i]);
                CAPTURE(forwards[i]);
                CAPTURE(back[i]);
                CHECK(forwards[i] > 0);
                CHECK(diff[i]< 1e-8);
            }
        }
    }
};
TEST_CASE_METHOD(StatelessVars, "Check all variables that do not require state information", "[Stateless]") { payload(); };

class MIXSTP : public REFPROPDLLFixture
{
public:
    void payload() {
        std::string flds = "Methane * Helium" + std::string(500, '\0');
        std::vector<double> z = {0.4, 0.6};
        auto r = REFPROP(flds, "PT", "ETA", 1, 0, 0, 0.101325, 298, z);
        CAPTURE(r.herr);
        REQUIRE(r.ierr == 0);
        double rho = r.Output[0];
        CHECK(rho == Approx(15).epsilon(0.1));
    }
};
TEST_CASE_METHOD(MIXSTP, "Mixture at STP", "[STP]") { payload(); };

class OneN2 : public REFPROPDLLFixture
{
public:
    void payload() {
        std::string flds = "Nitrogen" + std::string(500, '\0');
        std::vector<double> z(20, 1.0);
        auto r = REFPROP(flds, "TD&", "ETA", 0, 0, 0, 300, 0, z);
        CAPTURE(r.herr);
        CHECK(r.ierr == 0);
    }
};
TEST_CASE_METHOD(OneN2, "One call to transport for N2", "[OneN2]") { payload(); };

class CheckZEZEstimated : public REFPROPDLLFixture
{
public:
    void payload() {
        std::string flds = "R134A*R1234ZEZ" + std::string(500, '\0');
        std::vector<double> z = { 0.5,0.5 };
        auto r = REFPROP(flds, " ", "FIJMIX", 0, 0, 0, 1, 2, z);
        CAPTURE(r.herr);
        CHECK(r.ierr == -117);
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
        char herr[255];
        if (ierr != 0) {
            ERRMSGdll(ierr, herr, 255);
        }
        CAPTURE(herr);
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
            CAPTURE(fld);
            SETFLUIDSdll(cfld, ierr, 255);
            char herr[255];
            if (ierr != 0) {
                ERRMSGdll(ierr, herr, 255);
            }
            CAPTURE(herr);
            CHECK(ierr == 0);
        }
    }
};
TEST_CASE_METHOD(CheckAllLoads, "Check that all fluids load properly", "[setup]") { payload(); };

class CheckREDX : public REFPROPDLLFixture
{
public:
    void payload() {
        int ierr = 0;
        char fld[10000] = "Methane * Nitrogen";
        SETFLUIDSdll(fld, ierr, 255);
        CHECK(ierr == 0);
        double x[] = {0.5,0.5}, Tr=1e20, Dr = 1e20;
        REDXdll(x, Tr, Dr);
        CAPTURE(Tr);
        CAPTURE(Dr);
        CHECK(Tr > 50);
        CHECK(Tr < 250);
    }
};
TEST_CASE_METHOD(CheckREDX, "Check that REDX works properly", "[REDX]") { payload(); };

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
            char herrsetup[255] = "";
            if (ierr != 0) {
                ERRMSGdll(ierr, herrsetup, 255);
            }
            CAPTURE(herrsetup);
            REQUIRE(ierr == 0);

            double x[1] = {1.0};
            double p_kPa;
            char herr[255];
            SUBLTdll(pt.T_K, x, p_kPa, ierr,herr,255);
            double p_Pa = p_kPa*1000;
            CAPTURE(fld);
            CAPTURE(herr);
            REQUIRE(ierr == 0);
            CHECK(p_Pa == Approx(pt.p_Pa).epsilon(pt.eps_tol));
        }
    }
};
TEST_CASE_METHOD(SUBTVALIDATION, "Check sublimation pressures", "[sublimation]") { payload(); };