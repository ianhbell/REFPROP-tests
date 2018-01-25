#ifndef REFPROP_TESTS_UTILS
#define REFPROP_TESTS_UTILS

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 

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
