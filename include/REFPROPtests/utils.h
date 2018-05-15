#ifndef REFPROP_TESTS_UTILS
#define REFPROP_TESTS_UTILS

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 
#include <boost/algorithm/string/trim.hpp>
#include <locale>
#include <regex>
#include <string>

inline std::string get_file_contents(const std::string &filename) {
    using std::ios;
    std::ifstream ifs(filename.c_str(), ios::in | ios::binary);
    if (!ifs) {
        throw std::invalid_argument("filename cannot be opened: " + filename);
    }
    // See https://stackoverflow.com/a/116220/1360263
    return static_cast<std::stringstream const&>(std::stringstream() << ifs.rdbuf()).str();
}

/// Inspired by http://www.cplusplus.com/faq/sequences/strings/split/#getline
static std::vector<std::string> str_split(const std::string &s, 
                                          const std::string & delimiters = "\n"){
    int current;
    int next = -1;
    std::vector<std::string> o;
    do {
        current = next + 1;
        next = s.find_first_of(delimiters, current);
        o.push_back(s.substr(current, next - current));
    } while (next != std::string::npos);
    return o;
}

static std::vector<std::pair<std::string, std::string>> get_binary_pairs() {
    char* RPPREFIX = std::getenv("RPPREFIX");
    REQUIRE(strlen(RPPREFIX) != 0);
    namespace fs = boost::filesystem;

    fs::path target(fs::path(RPPREFIX) / fs::path("FLUIDS") / fs::path("HMX.BNC"));
    std::string contents = get_file_contents(target.string());
    std::regex pattern(R"(([\w \-]+\/[\w \-]+)\s{2,}\[)");
    std::smatch pair_match;
    std::vector<std::pair<std::string, std::string>> pairs;
    for (auto &&line : str_split(contents)) {
        if (std::regex_search(line, pair_match, pattern)){
            std::string flds = pair_match[1];
            boost::algorithm::trim(flds); // inplace
            auto fld = str_split(flds, "/");
            pairs.emplace_back(fld[0], fld[1]);
        }
    }
    return pairs;
}

/// From CoolProp
double string_to_double(const std::string &s){
    std::stringstream ss(s);
    char c = '.';

    struct delim : std::numpunct<char> {
        char m_c;
        delim(char c) : m_c(c) {};
        char do_decimal_point() const { return m_c; }
    };

    ss.imbue(std::locale(ss.getloc(), new delim(c)));
    double f;
    ss >> f;
    if (ss.rdbuf()->in_avail() != 0) {
        throw "fraction [%s] was not converted fully" + s;
    }
}

struct predef_mix_values {
    double Wmol, Tc, pc, rhoc;
    std::vector<double> molar_composition;
};
static predef_mix_values get_predef_mix_values(const std::string &fname) {
    char* RPPREFIX = std::getenv("RPPREFIX");
    REQUIRE(strlen(RPPREFIX) != 0);
    namespace fs = boost::filesystem;

    fs::path target(fs::path(RPPREFIX) / fs::path("MIXTURES") / fs::path(fname));
    std::string contents = get_file_contents(target.string());
    auto lines = str_split(contents);

    // Get Wmol and crit values
    auto val_string = str_split(lines[1], " ");
    std::vector<double> vals;
    for (auto &c : val_string) {
        boost::algorithm::trim(c); // inplace
        if (!c.empty()){
            vals.emplace_back(string_to_double(c));
        }
    }
    if (vals.size() != 4) {
        throw "Length of values is not 4 in "+ fname;
    }
    boost::algorithm::trim(lines[2]);
    std::vector<double> molar_compositions;
    auto Ncomp = static_cast<double>(string_to_double(lines[2]));
    for (auto i = 0; i < Ncomp; ++i) {
        boost::algorithm::trim(lines[3+Ncomp+i]);
        molar_compositions.emplace_back(string_to_double(lines[3 + Ncomp + i]));
    }
    return predef_mix_values{vals[0], vals[1], vals[2], vals[3], molar_compositions};
}

static std::vector<std::string> get_files_in_folder(const std::string &folder, const std::string &extension) {
    char* RPPREFIX = std::getenv("RPPREFIX");
    REQUIRE(strlen(RPPREFIX) != 0);
    namespace fs = boost::filesystem;
    std::vector<std::string> files;

    // See https://stackoverflow.com/a/8725664/1360263
    fs::path targetDir(fs::path(RPPREFIX) / fs::path(folder));
    fs::directory_iterator it(targetDir), eod;
    BOOST_FOREACH(fs::path const &p, std::make_pair(it, eod))
    {
        if (fs::is_regular_file(p) && p.extension() == fs::path(extension))
        {
            std::string s = fs::canonical(p).filename().replace_extension("").string();
            files.push_back(s);
        }
    }
    return files;
}

static std::vector<std::string> get_pure_fluids_list() {
    return get_files_in_folder("FLUIDS", ".FLD");
}

static std::vector<std::string> get_predefined_mixtures_list() {
    return get_files_in_folder("MIXTURES", ".MIX");
}

static std::vector<std::string> fluids_with_PH0() {
    std::vector<std::string> o;
    for (auto &fluid : get_pure_fluids_list()) {
        char* RPPREFIX = std::getenv("RPPREFIX");
        REQUIRE(strlen(RPPREFIX) != 0);
        namespace fs = boost::filesystem;
        std::string path = (fs::path(RPPREFIX) / fs::path("FLUIDS") / fs::path(fluid+".FLD")).string();
        std::string contents = get_file_contents(path);
        for (auto &line : str_split(contents)) {
            std::smatch match;
            if (std::regex_search(line, match, std::regex(R"(^PH0)"))) {
                o.emplace_back(fluid);
            }
        }
    }
    return o;
}

#endif
