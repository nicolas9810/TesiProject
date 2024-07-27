#include "GreenPlantModel.h"
#include <stdexcept>

const std::unordered_map<std::string, int> GreenPlantModel::csvColumMap = {
    {"Year", 0},
    {"Month", 1},
    {"Day", 2},
    {"Hour", 3},
    {"Minute", 4},
    {"Temperature", 5},
    {"Clearsky DHI", 6},
    {"Clearsky DNI", 7},
    {"Clearsky GHI", 8},
    {"Cloud Type", 9},
    {"Dew Point", 10},
    {"DHI", 11},
    {"DNI", 12},
    {"Fill Flag", 13},
    {"GHI", 14},
    {"Ozone", 15},
    {"Relative Humidity", 16},
    {"Solar Zenith Angle", 17},
    {"Surface Albedo", 18},
    {"Pressure", 19},
    {"Precipitable Water", 20},
    {"Wind Direction", 21},
    {"Wind Speed", 22}
};

const double GreenPlantModel::A1 = 2;
const double GreenPlantModel::A2 = 0;
const double GreenPlantModel::Cp = 0.75;
const double GreenPlantModel::R = 287.0500676;
const double GreenPlantModel::Wp = 180;
const double GreenPlantModel::Ct = 0.0038;
const double GreenPlantModel::BASE = 0;

GreenPlantModel::GreenPlantModel(const std::string &csvPath) {
    bool isFirstLine = true;
    std::ifstream file(csvPath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "File not found: " << csvPath << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string record;
        std::vector<std::string> records;
        while (std::getline(ss, record, ',')) {
            records.push_back(record);
        }

        try {
            double T = std::stod(records.at(csvColumMap.at("Temperature"))) + 273.15;
            double P = std::stod(records.at(csvColumMap.at("Pressure"))) * 100;
            double V = std::stod(records.at(csvColumMap.at("Wind Speed")));
            double h = std::stod(records.at(csvColumMap.at("GHI")));

            double Ps = h * A1 * Cp * Wp * A1 / 1000 * (1 + Ct * (298.15 - T));
            double Pw = 0.5 * (P / (R * T)) * A2 * Cp * std::pow(V, 3);
            energyValueList.push_back((Ps + BASE));
        } catch (const std::invalid_argument &e) {
            std::cerr << "Invalid argument at line: " << line << std::endl;
            std::cerr << e.what() << std::endl;
        } catch (const std::out_of_range &e) {
            std::cerr << "Out of range error at line: " << line << std::endl;
            std::cerr << e.what() << std::endl;
        }
    }
    #ifdef DEBUG_MODE
    std::cout<<"[MODELLO PANNELLO] Creato il modello"<<std::endl;
    #endif  
}

double GreenPlantModel::getProducedPowerByTime(double time) const {
    int location = static_cast<int>(std::round(time / 60));
    return energyValueList.at(location);
}
