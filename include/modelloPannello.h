#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>

class GreenPlantModel {
private:
    
    
public:
    // Colums map
    static const std::unordered_map<std::string, int> csvColumnMap;

    // Model parameters
    static constexpr double A1 = 1;
    static constexpr double A2 = 0;
    static constexpr double Cp = 0.75;
    static constexpr double R = 287.0500676;
    static constexpr double Wp = 180;
    static constexpr double Ct = 0.0038;
    static constexpr double BASE = 0;

    std::vector<double> energyValueList;

    GreenPlantModel();
    GreenPlantModel(const std::string &csvPath);

    double getProducedPowerByTime(double time) const;
};

