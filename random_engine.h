#ifndef RANDOM_ENGINE_H_
#define RANDOM_ENGINE_H_

// std
#include <vector>
#include <string>
#include <map>


class RandomEngine {
 public:
    RandomEngine() = default;

    void init();

    std::string getCi() const;

 private:
    void initCi();
    


 private:
    std::vector<std::string> m_ciList;
};

#endif  // RANDOM_ENGINE_H_
