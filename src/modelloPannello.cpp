#include "modelloPannello.h"


GreenPlantModel::GreenPlantModel() {}
GreenPlantModel::GreenPlantModel(const std::string &csvPath) {
    bool isFirstLine = true;
    std::ifstream file(csvPath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << csvPath << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> records;

        while (std::getline(ss, token, ',')) {
            records.push_back(token);
        }

        // Ensure the record has the expected number of columns
        if (records.size() < 23) {  // Assuming 23 is the expected number of columns
            std::cerr << "Invalid record length: " << line << std::endl;
            continue;
        }

        try {
            double T = std::stod(records.at(csvColumnMap.at("Temperature"))) + 273.15;
            double P = std::stod(records.at(csvColumnMap.at("Pressure"))) * 100;
            double V = std::stod(records.at(csvColumnMap.at("Wind Speed")));
            double h = std::stod(records.at(csvColumnMap.at("GHI")));
            double Ps = h * A1 * Cp * Wp * A1 / 1000 * (1 + Ct * (298.15 - T));
            //double Pw = 0.5 * (P / (R * T)) * A2 * Cp * std::pow(V, 3);
            energyValueList.push_back(Ps + BASE);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data in record: " << line << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Data out of range in record: " << line << std::endl;
            continue;
        }
    }

    file.close();
}

double GreenPlantModel::getProducedPowerByTime(double time) const {
    int location = static_cast<int>(std::round(time / 60));
    if (location < 0 || location >= energyValueList.size()) {
        std::cerr << "Time location out of bounds: " << location << std::endl;
        return 0.0;
    }
    // for (const double& value : energyValueList) {
    //     std::cout << value << " ";
    // }
    std::cout << std::endl;
    return energyValueList.at(location);
}

const std::unordered_map<std::string, int> GreenPlantModel::csvColumnMap = {
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

// int main() {
//     GreenPlantModel model("../csv/2019.csv");
//     double time = 720; // Example time
//     std::cout << "Produced power at time " << time << " minutes: " << model.getProducedPowerByTime(time) << std::endl;

//     time = 720+1440; // Example time
//     std::cout << "Produced power at time " << time << " minutes: " << model.getProducedPowerByTime(time) << std::endl;
    
//     time = 720+1440+1440; // Example time
//     std::cout << "Produced power at time " << time << " minutes: " << model.getProducedPowerByTime(time) << std::endl;
    
//     return 0;
// }
