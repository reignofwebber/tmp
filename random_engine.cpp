#include "random_engine.h"

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

void RandomEngine::init() {
    initCi();
    
}

void RandomEngine::initCi() {
    using boost::property_tree::ptree;
    ptree pt;
    boost::property_tree::read_xml("ci_config.xml", pt);
    for (auto &v : pt.get_child("CILIST")) {
        auto pt_ci = v.second;
        m_ciList.push_back(pt_ci.get<std::string>("<xmlattr>.id", ""));
    }
}






std::string RandomEngine::getCi() const {
    return m_ciList.at(rand() % m_ciList.size());
}

