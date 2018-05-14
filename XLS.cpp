#include "catch.hpp"
#include <utility>
#include "REFPROPtests/baseclasses.h"

// A structure to hold the values for one validation call
struct XLSin
{
    std::string OutputCode, FluidName, InpCode, Units;
    double Prop1, Prop2, Prop3;
    int Ninputs;
    XLSin(){};
    XLSin(const std::string &OutputCode, const std::string &FluidName) : OutputCode(OutputCode), FluidName(FluidName), Ninputs(2) {};
    template<typename T1> XLSin(const std::string &OutputCode, const std::string &FluidName, T1 Prop1) : OutputCode(OutputCode), FluidName(FluidName), Prop1(Prop1), Prop2(-99999999), Prop3(-99999999), Ninputs(3) {};
    template<typename T1> XLSin(const std::string &OutputCode, const std::string &FluidName, const std::string &InpCode, const std::string &Units, T1 Prop1) : OutputCode(OutputCode), FluidName(FluidName), InpCode(InpCode), Units(Units), Prop1(Prop1), Prop2(-99999999), Prop3(-99999999), Ninputs(5) {};
    template<typename T1, typename T2> XLSin(const std::string &OutputCode, const std::string &FluidName, const std::string &InpCode, const std::string &Units, T1 Prop1, T2 Prop2) : OutputCode(OutputCode), FluidName(FluidName), InpCode(InpCode), Units(Units), Prop1(Prop1), Prop2(Prop2), Prop3(-99999999), Ninputs(6) {};
    template<typename T1, typename T2, typename T3> XLSin(const std::string &OutputCode, const std::string &FluidName, const std::string &InpCode, const std::string &Units, T1 Prop1, T2 Prop2, T3 Prop3) : OutputCode(OutputCode), FluidName(FluidName), InpCode(InpCode), Units(Units), Prop1(Prop1), Prop2(Prop2), Prop3(Prop3), Ninputs(7) {};
};

class XLSChecksumFixture : public REFPROPDLLFixture {

public:
    std::vector<std::pair<double, XLSin>> inputs;

    XLSChecksumFixture(){

        inputs = {
            { 28.96546   ,{ "M","Air" } },
            { 637.3775887,{ "T","argon","PD","SI",2,15 } },
            { 1.456916965,{ "P","r134a","TD","SI",400,50 } },
            { 684.9965172,{ "H","ethylene","TS","SI",370,2.5 } },
            { 153.8866807,{ "VIS","oxygen","TP","SI",100,1 } },
            { 100.1117462,{ "TCX","nitrogen","Tliq","SI",100 } },
            { 0.07493553 ,{ "D","air","TP","E",70,14.7 } },
            { 23643.99395,{ "H","R32;R125|0.3;0.7","PS","molar si",10,110 } },
            { 298.4314139,{ "T","ethane;0.5;butane;0.5 mass","DH","E",30,283 } },
            { 5665.709969,{ "W","ammonia;0.4;water;0.6","TP","E",300,10000 } },
            { 1.600404294,{ "D","r218;.1;r123;.9","PH","cgs",7,180 } },
            { 0.038640593,{ "QMASS","methane;40;ethane;60 mass","TD","mks",200,300 } },
            { 0.038640617,{ "QMASS","methane;40;ethane;60 mass","TP","mks",200,2814.5509 } },
            { 0.050092664,{ "QMOLE","methane;ethane|.4;.6 mass","TP","molar SI",200,2.8145509 } },
            { 280.2337991,{ "VIS","butane;hexane|.25;.75","TH>","SI",300,-21 } },
            { 17.89148313,{ "TCX","carbon dioxide;nitrogen|.5;.5 mass","TS","molar SI",250,230.7 } },
            { 1.037058033,{ "DE","ethane;.5;propane;.5","tvap","SI",300 } },
            { 0.697614699,{ "xmole","R410A.MIX",1 } },
            { 0.5        ,{ "XMASS","R410A.MIX",1 } },
            { 439.4484635,{ "DLIQ","methane;ethane;propane;butane|0.8;0.15;0.03;0.02","TD","SI",150,200 } },
            { 49.71894258,{ "F(2)","propane;R125|10;90","TH>","E",-20,72 } },
            { 0.89854362 ,{ "FC(2)","N2;CO2|0.6;0.4","TQ","MKS",200,1 } },
            { 0.231604716,{ "xmass","air.mix","TD","molar SI",80,12,3 } },
            { 210.9797558,{ "T","butane","Ssat3","SI",2.5 } },
            { 17.30446997,{ "DLIQ","R410A.mix","TD","molar SI",250,10 } },
            { 0.768067188,{ "D","butane;propadiene|0.7;0.3 mass","TS","E",150,0.6 } },
            { 0.567871366,{ "dHdT_D","argon","TP","MIXED",150,200 } },
            { 999.9251311,{ "D","water","Tliq","C",4 } },
            { 0.004968591,{ "P","hexane;butane|.6;.4 mass","DE","SI",600,-120 } },
            { 33.24750174,{ "vis","argon;co2;propane;acetylene|0.8;0.15;0.03;0.02 mass","TD","SI",450,200 } },
            { 0.77364522 ,{ "qmass","argon;co2;propane;acetylene|0.8;0.15;0.03;0.02 mass","TH<","SI",160,55 } },
            { 369.89     ,{ "TCRIT(3)","methane;ethane;propane;butane|0.8;0.15;0.03;0.02" } },
            { 246.1166525,{ "P","MM;MDM;MD3M|0.8;0.15;0.05","TH<","E",500,177 } },
            { 30565.87495,{ "E","D4;D5;D6|0.8;0.15;0.05","TD","molar si",500,1 } },
            { 37.16306405,{ "E","XENON","HS","SI",44,0.175 } },
            { 14.2793971 ,{ "JT","CO2;WATER|0.5;0.5","Tvap","SI",400 } }
        };

        std::vector<double> zz(1, 1);
        auto res = REFPROP("", "flags", "SETREF", 0, 0, 2, 0, 0, zz);
        CHECK(res.ierr == 0);
    }

    void payload() {

        for (auto &theinput : inputs) {
            double expected; XLSin in;
            std::tie(expected, in) = theinput;
            switch (in.Ninputs) {
                case 2:
                {
                    char hFld[255]; strcpy(hFld, in.FluidName.c_str());
                    char hIn[255] = ""; 
                    char hOut[10000]; strcpy(hOut, in.OutputCode.c_str());
                    
                    int iUnit = 0, ierr = 0, iMass = 0, iFlag = 0;
                    double a = 0, b = 0, q = -1; double z[20], Output[200], x[20], y[20], x3[20]; char hUnits[255], herr[255]; 
                    int unit_system = get_enum("DEFAULT");
                    CAPTURE(in.FluidName);
                    CAPTURE(in.OutputCode);
                    REFPROPdll(hFld, hIn, hOut, unit_system, iMass, iFlag, a, b, z, Output, hUnits, iUnit, x, y, x3, q, ierr, herr, 10000, 255, 255, 255, 255);
                    CAPTURE(herr);
                    CHECK(ierr < 100);
                    CHECK(Output[0] == Approx(expected).epsilon(1e-5));
                    break;
                }
                case 3:
                {
                    char hFld[255]; strcpy(hFld, in.FluidName.c_str());
                    char hIn[255] = "";
                    char hOut[10000]; strcpy(hOut, (in.OutputCode + "(" + std::to_string(int(in.Prop1)) + ")").c_str());

                    int iUnit = 0, ierr = 0, iMass = 0, iFlag = 0;
                    double a = 0, b = 0, q = -1; double z[20], Output[200], x[20], y[20], x3[20]; char hUnits[255], herr[255];
                    int unit_system = get_enum("DEFAULT");
                    CAPTURE(hIn);
                    CAPTURE(hOut);
                    REFPROPdll(hFld, hIn, hOut, unit_system, iMass, iFlag, a, b, z, Output, hUnits, iUnit, x, y, x3, q, ierr, herr, 10000, 255, 255, 255, 255);
                    CAPTURE(herr);
                    CHECK(ierr < 100);
                    CHECK(Output[0] == Approx(expected).epsilon(1e-5));
                    break;
                }
                case 5:
                case 6:
                case 7:
                {
                    char hFld[255]; strcpy(hFld, in.FluidName.c_str());
                    char hIn[255]; strcpy(hIn, in.InpCode.c_str());
                    char hOut[10000]; 
                    if (in.Ninputs == 7){
                        strcpy(hOut, (in.OutputCode + "(" + std::to_string(int(in.Prop3)) + ")") .c_str());
                    }
                    else {
                        strcpy(hOut, in.OutputCode.c_str());
                    }

                    int iUnitOut = 0, ierr = 0, iMass = 0, iFlag = 0;
                    double a = in.Prop1, b = in.Prop2, q = -1; double z[20], Output[200], x[20], y[20], x3[20]; char hUnits[255], herr[255];
                    int unit_system  = get_enum(in.Units);
                    CAPTURE(unit_system);
                    CAPTURE(in.FluidName);
                    CAPTURE(in.Ninputs);
                    CAPTURE(hIn);
                    CAPTURE(hOut);
                    CAPTURE(in.Units);
                    CAPTURE(in.Prop1);
                    CAPTURE(in.Prop2);
                    if (in.Ninputs == 7) {
                        CAPTURE(in.Prop3);
                    }
                    
                    REFPROPdll(hFld, hIn, hOut, unit_system, iMass, iFlag, a, b, z, Output, hUnits, iUnitOut, x, y, x3, q, ierr, herr, 10000, 255, 255, 255, 255);
                    CAPTURE(herr);
                    CHECK(ierr < 100);
                    CHECK(Output[0] == Approx(expected).epsilon(1e-5));
                    break;
                }
                default: {
                    CHECK(in.Ninputs == 2);
                }
            }
        }
        

        //int ierr = 0, ncomp = 20, GERGon = 1; // Turn GERG 2008 on
        //char herr[255];
        //GERG08dll(ncomp, GERGon, ierr, herr, 255);
        //CHECK(ierr == 0);
        //std::string joined = components[0];
        //for (auto i = 1; i < components.size() - 1; ++i) {
        //    joined += " * " + components[i];
        //}

        //char hfld[10000];
        //strcpy(hfld, joined.c_str());
        //for (auto& data : GERG2008_validation_data) {
        //    std::vector<double> zperc = mixture_comps[data.GasNo - 2]; // the first Gas No is 2 -> index of 0 in vector   
        //    CHECK(zperc.size() == 21);
        //    std::vector<double> z(zperc.begin(), zperc.begin() + 20);
        //    CHECK(z.size() == 20);
        //    double sumz = 0;
        //    for (auto &_z : z) {
        //        _z /= 100.0;
        //        sumz += _z;
        //    }
        //    z[0] = 1 - (sumz - zperc[0] / 100.0);
        //    //CHECK(std::abs(sumz-1) < 1e-12);
        //    char hIn[255] = "TD&", hOut[255] = "P;CV;CP;W";
        //    int iMass = 0, iFlag = 0;
        //    double Output[200], x[20], y[20], x3[20];
        //    char hUnits[255]; int iUnit = 0;
        //    double q = 0;
        //    CAPTURE(data.T_K);
        //    CAPTURE(data.D_molL);
        //    CAPTURE(data.GasNo);
        //    REFPROPdll(hfld, hIn, hOut, MOLAR_SI, iMass, iFlag, data.T_K, data.D_molL, &(z[0]), Output, hUnits, iUnit, x, y, x3, q, ierr, herr, 10000, 255, 255, 255, 255);
        //    CAPTURE(herr);
        //    CHECK(ierr == 0);
        //    double p_err = std::abs(Output[0] - data.P_MPa);
        //    double cv_err = std::abs(Output[1] - data.cv_JmolK);
        //    double cp_err = std::abs(Output[2] - data.cp_JmolK);
        //    double w_err = std::abs(Output[3] - data.w_ms);
        //    CHECK(p_err < 2e-5);
        //    //CHECK(cv_err < 2e-4);
        //    //CHECK(cp_err < 2e-4);
        //    //CHECK(w_err < 2e-4);
        //}
    }
};
TEST_CASE_METHOD(XLSChecksumFixture, "Check checksum calculations from the Excel spreadsheet", "[Excel]") { payload(); };