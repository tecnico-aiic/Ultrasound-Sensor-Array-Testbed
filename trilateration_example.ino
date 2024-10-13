#include <vector>
#include <cmath>
#include <iostream>

// Structures of the sensor data
struct SensorNodes {
    double x, y; //Sensor node coordinates, the z axis is not implemented (yet)
    double objectDistance; // Measured distance from object to sensor
};


// Function to perform trilateration and calculate object coordinates
// - It receives 3 sensor objects and the variables to export the coordinates (x,y)
bool trilateration(const SensorNodes& s1, const SensorNodes& s2, const SensorNodes& s3, double& x, double& y) {
    double x1 = s1.x, y1 = s1.y, r1 = s1.objectDistance;
    double x2 = s2.x, y2 = s2.y, r2 = s2.objectDistance;
    double x3 = s3.x, y3 = s3.y, r3 = s3.objectDistance;

    double a = 2 * (x2 - x1);
    double b = 2 * (y2 - y1);
    double c = pow(r1, 2) - pow(r2, 2) - pow(x1, 2) + pow(x2, 2) - pow(y1, 2) + pow(y2, 2);
    double d = 2 * (x3 - x2);
    double e = 2 * (y3 - y2);
    double f = pow(r2, 2) - pow(r3, 2) - pow(x2, 2) + pow(x3, 2) - pow(y2, 2) + pow(y3, 2);
    
    double denominator = (a * e - b * d);

    if (denominator == 0) {
        return false; // No valid intersection of circles
    }

    x = (c * e - f * b) / denominator;
    y = (a * f - d * c) / denominator;

    return true;
}

// Function to calculate object coordinates using all combinations of 3 sensors from a set of 4 sensors
void calculateObjectCoordinatesAll(std::vector<SensorNodes>& sensorNodes) {
    if (sensorNodes.size() < 3) {
        Serial.println("Error: At least 3 sensor nodes are needed for trilateration.");
        return;
    }
    double sumX = 0, sumY = 0;
    int validCombinations = 0;

    // Loop through all combinations of 3 sensors
    for (size_t i = 0; i < sensorNodes.size() - 2; ++i) {
        for (size_t j = i + 1; j < sensorNodes.size() - 1; ++j) {
            for (size_t k = j + 1; k < sensorNodes.size(); ++k) {
                double x_object = 0, y_object = 0;

                // Perform trilateration on the current set of 3 sensors
                if (trilateration(sensorNodes[i], sensorNodes[j], 
                    sensorNodes[k], x_object, y_object)) {
                    sumX += x_object;
                    sumY += y_object;
                    validCombinations++;
                } else {
                    Serial.print("Error: Sensors ");
                    Serial.print(i);
                    Serial.print(", ");
                    Serial.print(j);
                    Serial.print(", and ");
                    Serial.print(k);
                    Serial.println(" do not intersect properly.");
                }
            }
        }
    }

    if (validCombinations > 0) {
        double avgX = sumX / validCombinations;
        double avgY = sumY / validCombinations;
        Serial.print("Estimated object coordinates: (");
        Serial.print(avgX);
        Serial.print(", ");
        Serial.print(avgY);
        Serial.println(")");
    } else {
        Serial.println("Error: No valid trilateration results.");
    }
}

// Arduino setup function
void setup() {
    // Start serial communication
    Serial.begin(9600);

    // Define the positions and distances of 4 sensor nodes
    std::vector<SensorNodes> sensorNodes = {
        {0, 0, 10}, // Sensor 1: (x, y, distance)
        {10, 0, 7}, // Sensor 2: (x, y, distance)
        {5, 8, 9}, // Sensor 3: (x, y, distance)
        {7, 7, 6} // Sensor 4: (x, y, distance)
    };

    // Calculate the object coordinates using trilateration for all combinations of 3 sensors
    calculateObjectCoordinatesAll(sensorNodes);
}

// Arduino loop function (empty as it's only for setup in this case)
void loop() {
    // Nothing to do here
}