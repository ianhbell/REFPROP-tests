#include <cstdlib>
#include <boost/algorithm/string/trim.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "REFPROPtests/baseclasses.h"
#include "REFPROPtests/utils.h"
#include <numeric>

TEST_CASE_METHOD(REFPROPDLLFixture, "Check NBP of water", "[nbp]"){
    int ierr = 1, nc = 1;
    char herr[255], hfld[255] = "WATER.FLD", hhmx[255] = "HMX.BNC", href[4] = "DEF";
    SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
    CHECK(ierr == 0);

    int kq = 1;
    double z[] = { 1.0 }, x[] = { 1.0 }, y[] = { 1.0 }, T = 300, p = 101.325, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = 0, w = -1;
    PQFLSHdll(p, q, z, kq, T, d, dl, dv, x, y, u, h, s, cp, cv, w, ierr, herr, 255);
    CAPTURE(herr);
    CHECK(ierr == 0);

    CHECK(T == Approx(373.15).margin(0.1));
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Try to load all predefined mixtures", "[setup],[predef_mixes]") {
    for (auto &&mix : get_predefined_mixtures_list()) {
        // Load it
        std::vector<double> z(20, 0.0);
        auto r = REFPROP(mix+".MIX", " ", "TRED", 1, 0, 0, 0.101325, 300, z);
        CAPTURE(mix+".MIX");
        CAPTURE(r.herr);
        CHECK(r.ierr < 100);
        // Get the predefined mixture critical values
        auto vals = get_predef_mix_values(mix+".MIX");
        // Turn on splines
        int ierr = 0; char herr[255] = "";
        SATSPLNdll(&(z[0]), ierr, herr, 255U);
        // Get critical point
        double Tcspl = -1, Pcspl = -1, Dcspl = -1;
        double Wmol; WMOLdll(&(z[0]), Wmol);
        CRITPdll(&(z[0]),Tcspl,Pcspl,Dcspl,ierr,herr,255U);
        CHECK(vals.Wmol == Approx(Wmol)); 
        CHECK(vals.Tc == Approx(Tcspl));
        CHECK(vals.pc == Approx(Pcspl));
        CHECK(vals.rhoc == Approx(Dcspl));
        // Check molar composition matches what we loaded
        for (auto i = 0; i < vals.molar_composition.size(); ++i) {
            CHECK(z[i] == Approx(vals.molar_composition[i]));
        }
    }
};

struct PRTvalue{
    std::string name;
    double P, e, h, s, Cv, Cp, w, hjt;
};

class PRTValues : public REFPROPDLLFixture {

public:
    std::vector<PRTvalue> values;

    PRTValues(){
        values = {
            { "Water", 664.5280477267937, 43035.59591183465, 45250.6894042573, 108.49384206969856, 25.613132127887006, 37.592214048055894, 397.54446474343916, 0.03201734475207269 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "R134a", 650.0863532227113,40570.2765828015,42737.23109354387,177.0472458415633,77.80366842408034,91.88953773737126,146.45824859887492,0.018503812298546574 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "R125", 666.5582366071258,40617.19554747021,42839.05633616063,188.53934651182294,87.16068192152147,100.28201827764514,137.10402282246275,0.013855649237427176 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "R32", 689.5688079557667,26597.210341545142,28895.77303473103,120.74745119445683,35.197549998816484,46.468053459300315,231.3342557575627,0.019097476605463515 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "Oxygen", 744.0959636690404,6187.147932994567,8667.467811891369,188.47141068953127,21.130313557754306,29.79997242389251,329.7224876590427,0.003001816856186118 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "Nitrogen", 746.9198561991534,6181.056953198343,8670.78980719552,174.91015048214658,20.881876393724724,29.519732336576613,354.16374692556656,0.002377049562837003 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "IOCTANE", 335.05048765100963,11426.144458078626,12542.979416915325,19.313792529870902,179.4520391885055,-67.83132968331836,22.396528812701423,1.136266574781018 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "Hydrogen", 751.0055715311174,5518.583226302729,8021.935131406453,93.20087546247652,20.53415337157465,28.898914072411927,1324.3949408542335,-7.077550194483265e-05 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "Helium", 750.5659596974368,3754.7788227019937,6256.665355026784,94.49020914450256,12.469015511006797,20.797959171736995,1022.6402482720125,-0.00034803585800532386 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "Ethanol", 561.1846228123452,32185.498918476784,34056.11432785127,82.88998277489753,58.61827224817137,82.5914745200324,197.05251773167913,0.05569893032657596 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "C12", -332.8388031040553,-30346.238589564367,-31455.701266577886,-102.43088941108591,277.4538276921272,259.0212552566496,0.0,-0.027526499530279865 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
            { "Argon", 744.1577242095819,3689.9157608508503,6170.441508216123,138.11712930970924,12.524550335809662,21.182803625340483,323.19179311520134,0.004102973433556607 }, //  P,e,h,s,Cv,Cp,w,hjt @ T=300K, D=0.3 mol/L
        };
    };
    void payload() {
        for (auto &val : values) {
            int nc = 1;
            {
                int ierr = 0;
                char h1[4] = "EOS", h2[4] = "NBS", h3[255] = "PRT", herr[255] = "";
                SETMODdll(nc, h1, h2, h3, ierr, herr, 3, 3, 255, 255);
            }
            {
                int ierr; char herr[255]="", hfld[10000], hhmx[255] = "HMX.BNC", href[4] = "DEF";
                strcpy(hfld, (val.name + ".FLD").c_str());
                SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
            }
            double P, e, h, s, Cv, Cp, w, hjt;
            double T = 300, D = 0.3, z[20] = {1.0};
            THERMdll(T, D, z, P, e, h, s, Cv, Cp, w, hjt);
            CHECK(P == Approx(val.P).epsilon(1e-2));
            CHECK(e == Approx(val.e).epsilon(1e-2));
            CHECK(h == Approx(val.h).epsilon(1e-2));
            CHECK(s == Approx(val.s).epsilon(1e-2));
            CHECK(Cv == Approx(val.Cv).epsilon(1e-2));
            CHECK(Cp == Approx(val.Cp).epsilon(1e-2));
            CHECK(w == Approx(val.w).epsilon(1e-2));
            CHECK(hjt == Approx(val.hjt).epsilon(1e-2));
        }
    }
};
TEST_CASE_METHOD(PRTValues, "CHECK 9.1.1 values w/ PRT model", "[setup],[911]") { payload(); }

TEST_CASE_METHOD(REFPROPDLLFixture, "CHECK 9.1.1 values w/ 4-component refrigerant mixture", "[flash],[911]") {
    std::vector<double> z(20, 0.0); for (auto i = 0; i < 4; ++i){ z[i] = 0.25; }
    std::vector<double> vals_911 = { 1623.7467678192363, 33190.76537542725, 34814.51214324648, 156.16069343722648, 90.2775245338805, 162.39122497807574, 128.3393178811631, 0.025476123512775832 }; // p,e,h,s,cv,cp,w,htj @ 300 K, 1.0 mol/L
    std::vector<std::string> keys = { "p","e","h","s","cv","cp","w", "JT" };
    REQUIRE(std::accumulate(z.begin(), z.end(), 0.0) == Approx(1.0));
    {
        reload();
        INFO("Default flags w/ Rgas=2");
        auto r0 = REFPROP("R32 * R125 * R134A * R143A", "TD&", "W", 0, 0, 0, 300, 1.0, z);
        REQUIRE(r0.ierr == 0);
        int k = -1;
        FLAGS("GAS CONSTANT", 2, k);

        for (auto i = 0; i < keys.size(); ++i) {
            CAPTURE(keys[i]);
            auto r = REFPROP("", "TD&", keys[i]+";R", 0, 0, 0, 300, 1.0, z);
            REQUIRE(r.ierr == 0);
            double R = r.Output[1];
            CAPTURE(R);
            auto v = vals_911[i];
            CHECK(r.Output[0] == Approx(v));
        }
    }
    {
        reload();
        INFO("w/ PX0=1 & w/ Rgas=2");
        auto r0 = REFPROP("R32 * R125 * R134A * R143A * R152A", "", "", 0, 0, 0, 300, 1.0, z); 
        CAPTURE(r0.herr);
        REQUIRE(r0.ierr == 0);
        int k = -1;
        FLAGS("PX0", 1, k);
        FLAGS("GAS CONSTANT", 2, k);
        for (auto i = 0; i < keys.size(); ++i) {
            CAPTURE(keys[i]);
            auto r = REFPROP("", "TD&", keys[i], 0, 0, 0, 300, 1.0, z);
            REQUIRE(r.ierr == 0);
            auto v = vals_911[i];
            CHECK(r.Output[0] == Approx(v));
        }
    }
}

TEST_CASE_METHOD(REFPROPDLLFixture, "Unset splines", "[flags]") {
    std::vector<double> z(20, 1.0); z[0] = 0.4; z[1] = 0.6;
    
    // Do normal calc for CRITP
    auto r0 = REFPROP("R32 * PROPANE", "", "TC", 0, 0, 0, 0, 0, z);

    // Same calc (with splines)
    auto r1 = REFPROP("R32 * PROPANE", "", "TC", 0, 0, 1, 0, 0, z);

    // Turn off splines, back to original
    int k = 0;
    FLAGS("SPLINES OFF", 1, k);
    FLAGS("SpLiNeS oFf", 1, k); // check case sensitivity (should not be case sensitive)

    // Final result, should be same as r0
    auto r2 = REFPROP("R32 * PROPANE", "", "TC", 0, 0, 0, 0, 0, z);

    CHECK(r0.ierr == r2.ierr);
    CHECK(r0.Output[0] == Approx(r2.Output[0]).epsilon(1e-14));
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Unset bounds", "[flags]") {
    std::vector<double> z(20, 1.0); z[0] = 0.4; z[1] = 0.6;

    // Do normal calc for propane below Ttrip
    auto r0 = REFPROP("PROPANE", "TQ", "D", 0, 0, 0, 80, 0, z);
    
    // Turn off bounds
    int k = 0;
    FLAGS("BOUNDS", 1, k);
    FLAGS("BoUnDs", 1, k); // check case sensitivity (should not be case sensitive)

    // Same calc (with bounds off)
    auto r1 = REFPROP("PROPANE", "TQ", "D", 0, 0, 0, 80, 0, z);

    CHECK(r0.ierr == 1);
    CHECK(r1.ierr == 0);
    CHECK(r0.Output[0] == Approx(r1.Output[0]).epsilon(1e-14));
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check AGA8 stays on after SETFLUIDS", "[flags],[AGA8],[AGA8stayon]") {
    std::vector<double> z(20, 1.0); z[0] = 0.4; z[1] = 0.6;

    // Do normal calc w/ full EOS
    auto r0 = REFPROP("Methane * Ethane", "TQ", "D", 0, 0, 0, 80, 0, z);

    // Turn on AGA8 flag
    auto r1 = REFPROP("", "flags", "aga8", 1, 0, 1, 80, 0, z);

    // Query the AGA8 flag
    int k0 = 0;
    FLAGS("AGA8", -999, k0);
    REQUIRE(k0 == 1);

    // Set fluids
    {
        int ierr = 0; char cfld[10000] = "Methane * Ethane";
        SETFLUIDSdll(cfld, ierr, 255);
        char herrsetup[255] = "";
        if (ierr != 0) {
            ERRMSGdll(ierr, herrsetup, 255);
        }
        CAPTURE(herrsetup);
        CHECK(ierr == 0);
    }

    // Check AGA8; should still be on
    int k = 0;
    FLAGS("AGA8", -999, k);
    REQUIRE(k == 1);
};



TEST_CASE_METHOD(REFPROPDLLFixture, "Test all PX0 for pures", "[setup],[PX0]") {
    auto with_PH0 = fluids_with_PH0_or_PX0();
    REQUIRE(with_PH0.size() > 0);
    for (auto &&fluid : with_PH0) {
        std::vector<double> z(20,1.0);
        auto r = REFPROP(fluid, " ", "TRED;DRED", 1, 0, 0, 0, 0, z);
        double tau = 0.9, delta = 1.1, rho = delta*r.Output[1], T = r.Output[0] / tau;
        CAPTURE(fluid);
        CHECK(r.ierr < 100);

        reload();
        r = REFPROP(fluid, "TD&", "PHIG00;PHIG10;PHIG11;PHIG01;PHIG20", 1, 0, 0, T, rho, z);
        CHECK(r.ierr == 0);
        std::vector<double> default_ = std::vector<double>(r.Output.begin(), r.Output.begin() + 5);

        reload();
        {
            char hflag[255] = "PX0", herr[255] = "";
            int jflag = 0, kflag = -1, ierr = 0;
            FLAGSdll(hflag, jflag, kflag, ierr, herr, 255, 255);
            CAPTURE(herr);
            REQUIRE(ierr == 0);
            REQUIRE(kflag == jflag);
        }
        r = REFPROP(fluid, "TD&", "PHIG00;PHIG10;PHIG11;PHIG01;PHIG20", 1, 0, 0, T, rho, z);
        CHECK(r.ierr == 0);
        std::vector<double> normal = std::vector<double>(r.Output.begin(), r.Output.begin() + 5); 

        reload();
        {
            char hflag[255] = "PX0", herr[255] = "";
            int jflag = 1, kflag = -1, ierr = 0;
            FLAGSdll(hflag, jflag, kflag, ierr, herr, 255, 255);
            CAPTURE(herr);
            REQUIRE(ierr == 0);
            REQUIRE(kflag == jflag);
        }
        r = REFPROP(fluid, "TD&", "PHIG00;PHIG10;PHIG11;PHIG01;PHIG20", 1, 0, 0, T, rho, z);
        CHECK(r.ierr == 0);
        std::vector<double> w_PH0 = std::vector<double>(r.Output.begin(), r.Output.begin()+5);

        CHECK(normal.size() == w_PH0.size());
        CHECK(default_.size() == w_PH0.size());
        for (auto i = 0; i < normal.size(); ++i) {
            CHECK(normal[i] == Approx(default_[i]).margin(1e-8)); 
            CHECK(normal[i] == Approx(w_PH0[i]).margin(1e-6));
        }
    }
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Test PX0 for mixtures", "[setup],[PX0],[PX0mix]") {
    auto with_PH0 = fluids_with_PH0_or_PX0();
    REQUIRE(with_PH0.size() > 0);
    int Ncomp = 5;
    std::vector<double> z(Ncomp, 1/static_cast<double>(Ncomp)); 
    int mixes_run = 0;
    for (auto i0 = 0; i0 < with_PH0.size(); ++i0) {

        // Build fluid string
        std::string fluids = with_PH0[i0];
        for (auto j = 1; j < Ncomp; ++j){
            auto ii = (i0 + j) % (with_PH0.size() - 1); // mod to wrap around
            fluids += " * " + with_PH0[ii];
        }
        
        // Can you load and get reducing state?
        auto r = REFPROP(fluids, " ", "TRED;DRED", 1, 0, 0, 0, 0, z);
        double tau = 0.9, delta = 1.1, rho = delta*r.Output[1], T = r.Output[0] / tau;
        if (r.ierr > 100) {
            continue;
        }
        CAPTURE(fluids);
        CHECK(r.ierr < 100);
        mixes_run++;

        reload();
        r = REFPROP(fluids, "TD&", "PHIG00;PHIG10;PHIG11;PHIG01;PHIG20", 1, 0, 0, T, rho, z);
        CHECK(r.ierr < 100);
        std::vector<double> default_ = std::vector<double>(r.Output.begin(), r.Output.begin() + 5);

        reload();
        {
            char hflag[255] = "PX0", herr[255] = "";
            int jflag = 0, kflag = -1, ierr = 0;
            FLAGSdll(hflag, jflag, kflag, ierr, herr, 255, 255);
            CAPTURE(herr);
            REQUIRE(ierr == 0);
            REQUIRE(kflag == jflag);
        }
        r = REFPROP(fluids, "TD&", "PHIG00;PHIG10;PHIG11;PHIG01;PHIG20", 1, 0, 0, T, rho, z);
        CHECK(r.ierr < 100);
        std::vector<double> normal = std::vector<double>(r.Output.begin(), r.Output.begin() + 5);

        reload();
        {
            char hflag[255] = "PX0", herr[255] = "";
            int jflag = 1, kflag = -1, ierr = 0;
            FLAGSdll(hflag, jflag, kflag, ierr, herr, 255, 255);
            CAPTURE(herr);
            REQUIRE(ierr == 0);
            REQUIRE(kflag == jflag);
        }
        r = REFPROP(fluids, "TD&", "PHIG00;PHIG10;PHIG11;PHIG01;PHIG20", 1, 0, 0, T, rho, z);
        CHECK(r.ierr < 100);
        std::vector<double> w_PH0 = std::vector<double>(r.Output.begin(), r.Output.begin() + 5);

        CHECK(normal.size() == w_PH0.size());
        CHECK(default_.size() == w_PH0.size());
        for (auto i = 0; i < normal.size(); ++i) {
            CHECK(normal[i] == Approx(default_[i]).margin(1e-8));
            CHECK(normal[i] == Approx(w_PH0[i]).margin(1e-6));
        }
    }
    REQUIRE(mixes_run > 0); // Make sure at least some tests ran
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check mysterious bug with molar mass", "[R134AMM]") {
    std::vector<double> z0(20,0.0); z0[0] = 1.0;
    int MOLAR_BASE_SI = get_enum("MOLAR BASE SI");
    //auto r0 = REFPROP("Water", "PQ", "T", MOLAR_BASE_SI, 0, 0, 101325, 0, z0);
    /*int DEFAULT = get_enum("DEFAULT");
    int SI = get_enum("SI");
    MOLAR_BASE_SI = get_enum("MOLAR BASE SI");*/
    std::vector<double> z1(20, 0.0); z1[0] = 1.0;
    auto r1 = REFPROP("R134A", "PQ", "H;S;G;R;M;W", MOLAR_BASE_SI, 0, 0, 101325, 0, z1);
    REQUIRE(r1.Output[4] == Approx(0.10203).epsilon(1e-3));
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Molar mass of R134a", "[file_loading],[setup]") {
    std::vector<double> z = { 1.0 };
    int MOLAR_BASE_SI = get_enum("MOLAR BASE SI");
    int MOLAR_SI = get_enum("MOLAR SI");
    REQUIRE(REFPROP("R134A", " ", "M", MOLAR_BASE_SI, 0, 0, 0, 0, z).Output[0] == Approx(REFPROP("R134A", " ", "M", MOLAR_SI, 0, 0, 0, 0, z).Output[0] / 1000));
    REQUIRE(REFPROP("R134A", " ", "M", MOLAR_BASE_SI, 0, 0, 0, 0, z).Output[0] == Approx(0.10203).epsilon(1e-3));
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check fluid files with dash in them", "[file_loading],[setup]") {
    int ierr = 1, nc = 1;
    char herr[255], hfld[] = "-10.0.FLD", hhmx[] = "HMX.BNC", href[] = "DEF";
    char nopath[255] = "";
    SETPATHdll(nopath, 255);
    SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
    CAPTURE(herr);
    REQUIRE(ierr == 0);
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check TP flash of multicomponent mixture", "[flash]"){
    char hfld[10000] = "NITROGEN|WATER|CO2|H2S|METHANE|ETHANE|PROPANE|ISOBUTAN|BUTANE|IPENTANE|PENTANE";
    double z[] = { 1.2000036000108E-03,7.000021000063E-06,.828792486377459,2.000006000018E-04,.160400481201444,7.6000228000684E-03,1.4000042000126E-03,1.000003000009E-04,2.000006000018E-04,0,1.000003000009E-04 };
    int ierr = 0, nc = 11;
    char herr[255], hhmx[] = "HMX.BNC", href[] = "DEF";
    SETUPdll(nc, hfld, hhmx, href, ierr, herr, 10000, 255, 3, 255);
    REQUIRE(ierr == 0);

    ierr = 0;
    double x[30] = { 1.0 }, y[20] = { 1.0 }, T = 313.15, p = 400, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = 0, w = -1;
    TPFLSHdll(T, p, z, d, dl, dv, x, y, q, u, h, s, cp, cv, w, ierr, herr, 255);
    CAPTURE(herr);
    REQUIRE(ierr == 0);
};

TEST_CASE_METHOD(REFPROPDLLFixture, "EnablePR", "[setup]") {
    // SETUP
    int ierr = 0, nc = 2;
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
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check NBP for all pure fluids (when possible)", "[flash]"){ 
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

        double wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas, z[] = { 1.0 };
        int icomp = 1, kq = 1;
        INFOdll(icomp, wmm, ttrp, tnbpt, tc, pc, Dc, Zc, acf, dip, Rgas);

        double T, p = 101.325, D, Dl, Dv, xliq[20], xvap[20], q = 0, u, h, s, cv, cp, w;
        int ierr = 0; char herr[255] = "";

        char htyp[4] = "EOS"; double Tmin, Tmax, Dmax, Pmax;
        LIMITSdll(htyp, z, Tmin, Tmax, Dmax, Pmax, 3);

        // Calculate T_nbp given p_nbp (101.325 kPa)
        PQFLSHdll(p, q, z, kq, T, D, Dl, Dv, xliq, xvap, u, h, s, cv, cp, w, ierr, herr, 255);
        CAPTURE(fld);
        CAPTURE(herr);
        CHECK(ierr < 100);
        if (T < Tmin) { continue; }
        CHECK(T == Approx(tnbpt).margin(0.01));

        // Now do the inverse calculation T_nbp->p_nbp
        T = tnbpt; ierr = 0;
        TQFLSHdll(T, q, z, kq, p, D, Dl, Dv, xliq, xvap, u, h, s, cv, cp, w, ierr, herr, 255);
        CAPTURE(fld);
        CAPTURE(herr);
        CHECK(ierr < 100);
        CHECK(p == Approx(101.325).margin(0.1));
    }
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check Ttriple for all pure fluids (when possible)", "[flash]"){
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
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check absolute paths are ok", "[setup]") {
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
    double wmol = 0; int i = 1;
    WMOLIdll(i, wmol);
    CHECK(wmol > 26);
    CHECK(wmol < 27);
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check full absolute paths are ok", "[setup]") {
    int MOLAR_BASE_SI = get_enum("MOLAR BASE SI");

    std::string hmx = std::string(std::getenv("RPPREFIX")) + "/FLUIDS/HMX.BNC";
    // hmx += std::string(255-hmx.size(), '\0');
    char * hhmx = const_cast<char *>(hmx.c_str());

    for (char sep : {'*', ';'})
    {
        std::string fld0 = std::string(std::getenv("RPPREFIX")) + "FLUIDS/R32.FLD";
        std::string fld1 = std::string(std::getenv("RPPREFIX")) + "FLUIDS/PROPANE.FLD";
        std::string fld = fld0 + std::string(1, sep) + fld1;
        std::string flds = fld + std::string(10000 - fld.size(), '\0');
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
        flds += std::string(10000 - flds.size(), '\0');
        char * hfld = const_cast<char *>(flds.c_str());
        int ierr = 0, nc = 2; char hdef[4] = "DEF", herr[255];
        SETUPdll(nc, hfld, hhmx, hdef, ierr, herr, 10000, 255, 3, 255);
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
};

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

TEST_CASE_METHOD(REFPROPDLLFixture, "Critical TC1, VC1", "[crit]") {
    char cfld[255] = "Methane * Ethane";
    int ierr = 0;
    SETFLUIDSdll(cfld, ierr, 255);
    char herr[255];
    if (ierr != 0) {
        ERRMSGdll(ierr, herr, 255);
    }
    CAPTURE(herr);

    double z[20] = { 0.5,0.5 };
    ierr = 0;
    char herr2[255] = "";
    double Tcrit, pcrit_kPa, dcrit_mol_L;
    CRITPdll(z, Tcrit, pcrit_kPa, dcrit_mol_L, ierr, herr2, 255);
    CAPTURE(herr2);
    CHECK(ierr <= 0); 
};

TEST_CASE_METHOD(REFPROPDLLFixture, "CAS# for PROPANE", "[CAS]") { 
    std::vector<double> z = {1.0};
    auto r = REFPROP("PROPANE", " ", "CAS#", 0,0,0,0,0,z);
    CAPTURE(r.herr);
    CHECK(r.ierr == 0);
    boost::algorithm::trim(r.hUnits); //inplace
    CHECK(r.hUnits == "74-98-6");
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Reload the DLL 100 times", "[100Reloads]") {
    for (auto i =0; i < 100; ++i){ 
        reload(); 
    }
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Torture test calling of DLL", "[Torture]") {
    {
        char hflag[255] = "Debug", herr[255] = "";
        int jflag = 1, kflag = -1, ierr = 0;
        FLAGSdll(hflag, jflag, kflag, ierr, herr, 255, 255);
    }
    for (auto &&pair : get_binary_pairs()) {
        for (std::string &&k : { "ETA", "TCX", "CP", "P", "STN", "TC", "PC" }) {
            for (bool forwards : {true, false}) {
                std::vector<double> z;
                if (forwards) {
                    z = { 0.4,0.6 };
                }
                else {
                    z = { 0.6,0.4 };
                }
                std::string fluids = (forwards) ? pair.first + "*" + pair.second : pair.second + "*" + pair.first;
                REFPROP(fluids, "PT", k, 1, 0, 0, 0.101325, 300, z);
            }
        }
    }
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check all alphar derivatives", "[alphar]") {
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

        double tau = 1.3, delta = 0.9;
        std::vector<double> z = { 0.0, 1.0 };
        auto deriv = [this, &z](int itau, int idelta, double tau, double delta) {
            double arderiv;
            PHIXdll(itau, idelta, tau, delta, &(z[0]), arderiv);
            return arderiv / (pow(tau, itau)*pow(delta, idelta));
        };
        for (int itau = 0; itau < 3; ++itau) {
            for (int idelta = 0; idelta < 3; ++idelta) {
                if (itau + idelta > 4 || itau + idelta == 0) {
                    continue;
                }
                // centered derivative based on the lower-order derivative
                double numeric;
                if (itau > 0) {
                    double dtau = 1e-6;
                    numeric = (-deriv(itau - 1, idelta, tau + 2 * dtau, delta)
                        + 8 * deriv(itau - 1, idelta, tau + dtau, delta)
                        - 8 * deriv(itau - 1, idelta, tau - dtau, delta)
                        + deriv(itau - 1, idelta, tau - 2 * dtau, delta)) / (12 * dtau);
                }
                else {
                    double ddelta = 1e-6;
                    numeric = (-deriv(itau, idelta - 1, tau, delta + 2 * ddelta)
                        + 8 * deriv(itau, idelta - 1, tau, delta + ddelta)
                        - 8 * deriv(itau, idelta - 1, tau, delta - ddelta)
                        + deriv(itau, idelta - 1, tau, delta - 2 * ddelta)) / (12 * ddelta);
                }
                double analytic = deriv(itau, idelta, tau, delta);
                CAPTURE(itau);
                CAPTURE(idelta);
                CAPTURE(joined);
                CHECK(analytic == Approx(numeric).epsilon(1e-6));
            }
        }
    }
};

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

TEST_CASE_METHOD(REFPROPDLLFixture, "Check all variables that do not require state information", "[Stateless]") {
    // These need splines on...
    //std::vector<std::string> variable_names = { "TMAXT","PMAXT","DMAXT","TMAXP","PMAXP","DMAXP"};

    std::vector<std::string> variable_names = { "TC","PC","DC","TCEST","PCEST","DCEST","TRED","DRED","TMIN","TMAX","DMAX","PMAX" };
    std::string joined = variable_names[0];
    for (auto i = 1; i < variable_names.size(); ++i) {
        joined += ";" + variable_names[i];
    }
    for (auto &&pair : get_binary_pairs()) {
        std::vector<double> zz(20,1.0);
        if (REFPROP(pair.first + "*" + pair.second, " ", joined, 0, 0, 0, 0, 0, zz).ierr == 101) {
            WARN("Unable to load the fluids:"+ pair.first + "*" + pair.second);
            continue;
        }
        auto get_Tred = [this, &pair, &joined, &variable_names](bool forwards) {
            auto flds = (forwards) ? pair.first + "*" + pair.second : pair.second + "*" + pair.first;
            std::vector<double> z;
            if (forwards) {
                z = { 0.4, 0.6 };
            }
            else {
                z = { 0.6, 0.4 };
            };
            auto Nvars = variable_names.size();
            auto r = REFPROP(flds, " ", joined, 0, 0, 0, 0, 0, z);
            CAPTURE(r.herr);
            CHECK(r.ierr < 100);
            return std::vector<double>(r.Output.begin(), r.Output.begin() + Nvars);
        };
        auto forwards = get_Tred(true), back = get_Tred(false), diff = forwards;
        for (auto i = 0; i < forwards.size(); ++i) {
            diff[i] = std::abs(forwards[i] - back[i]);
            CAPTURE(forwards[i]);
            CAPTURE(back[i]);
            CHECK(forwards[i] > 0);
            CHECK(diff[i]< 1e-8);
        }
    }
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Mixture at STP", "[STP]") {
    std::string flds = "Methane * Helium" + std::string(500, '\0');
    std::vector<double> z = { 0.4, 0.6 };
    auto r = REFPROP(flds, "PT", "ETA", 1, 0, 0, 0.101325, 298, z);
    CAPTURE(r.herr);
    REQUIRE(r.ierr == 0);
    double rho = r.Output[0];
    CHECK(rho == Approx(15).epsilon(0.1));
};

TEST_CASE_METHOD(REFPROPDLLFixture, "One call to transport for N2", "[OneN2]") {
    std::string flds = "Nitrogen" + std::string(500, '\0');
    std::vector<double> z(20, 1.0);
    auto r = REFPROP(flds, "TD&", "ETA", 0, 0, 0, 300, 0, z);
    CAPTURE(r.herr);
    CHECK(r.ierr == 0);
};

TEST_CASE_METHOD(REFPROPDLLFixture, "CheckZEZEstimated", "[setup]") {
    std::string flds = "R134A*R1234ZEZ" + std::string(500, '\0');
    std::vector<double> z = { 0.5,0.5 };
    auto r = REFPROP(flds, " ", "FIJMIX", 0, 0, 0, 1, 2, z);
    CAPTURE(r.herr);
    CHECK(r.ierr == -117);
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check that R1234ze is an invalid fluid name", "[setup]") {
    char cfld[255] = "R1234ze";
    int ierr = 0;
    SETFLUIDSdll(cfld, ierr, 255);
    char herr[255];
    if (ierr != 0) {
        ERRMSGdll(ierr, herr, 255);
    }
    CAPTURE(herr);
    CHECK(ierr == 851);
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check that all fluids load properly", "[setup]") {
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
};

TEST_CASE_METHOD(REFPROPDLLFixture, "Check that REDX works properly", "[REDX]") {
    int ierr = 0;
    char fld[10000] = "Methane * Nitrogen";
    SETFLUIDSdll(fld, ierr, 255);
    CHECK(ierr == 0);
    double x[] = { 0.5,0.5 }, Tr = 1e20, Dr = 1e20;
    REDXdll(x, Tr, Dr);
    CAPTURE(Tr);
    CAPTURE(Dr);
    CHECK(Tr > 50);
    CHECK(Tr < 250);
};

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
        //{"HYDROGEN", 8.76, 44.4713821784, 0.001 }, NOT USED, updated sublimation curve is questionable
        {"NEON", 20, 3.81191E3,0.001 },
        {"D2O", 245, 0.327390934e2, 0.002} // IAPWS from Herrig
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