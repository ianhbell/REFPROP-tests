#ifndef REFPROP_TESTS_UTILS
#define REFPROP_TESTS_UTILS

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 
#include <boost/algorithm/string/trim.hpp>

#include <regex>

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
    std::string spattern = R"(([\w \-]+\/[\w \-]+)\s{2,}\[)";
    std::regex pattern(spattern);
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

static std::vector<std::string> get_pure_fluids_list() {
    char* RPPREFIX = std::getenv("RPPREFIX");
    REQUIRE(strlen(RPPREFIX) != 0);
    namespace fs = boost::filesystem;
    std::vector<std::string> fluids;

    // See https://stackoverflow.com/a/8725664/1360263
    fs::path targetDir(fs::path(RPPREFIX) /  fs::path("FLUIDS"));
    fs::directory_iterator it(targetDir), eod;
    BOOST_FOREACH(fs::path const &p, std::make_pair(it, eod))
    {
        if (fs::is_regular_file(p) && p.extension() == fs::path(".FLD"))
        {
            std::string s = fs::canonical(p).filename().replace_extension("").string();
            fluids.push_back(s);
        }
    }
    return fluids;
}

#endif
