#ifndef GREENPLANTMODEL_H
#define GREENPLANTMODEL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <string>

class GreenPlantModel {
private:
    static const std::unordered_map<std::string, int> csvColumMap;
    static const double A1;
    static const double A2;
    static const double Cp;
    static const double R;
    static const double Wp;
    static const double Ct;
    static const double BASE;
    std::vector<double> energyValueList;

public:
    GreenPlantModel(const std::string &csvPath);
    double getProducedPowerByTime(double time) const;
};

#endif // GREENPLANTMODEL_H
