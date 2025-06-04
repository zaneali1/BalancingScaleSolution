#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility> 
#include <cctype>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

struct PanNode {
    string left;
    string right;
};

vector<string>              scaleNames;
map<string, PanNode>        scaleNamesToNodes;
map<string, int>            calculatedMasses; // Memoisation for calculated masses.
map<string, pair<int, int>> imbalanceFixes;   // Stores the solution to the problem 
// (i.e. additional weights needed for balancing).

/**
 * @brief Determines if a given string represents a numeric mass.
 *
 * @param val String input to check.
 * @return true if val is a positive integer mass.
 * @return false otherwise (i.e. a scale name).
 */
bool isMassInt(const string& item) {
    return all_of(item.begin(), item.end(), ::isdigit);
}

/**
 * @brief Recursively computes the total mass of a given item.
 * Uses memoization to avoid repeated computation.
 *
 * @param item Mass or scale name.
 * @return int Total mass of the item.
 */
int calculateTotalMass(const string& item) {

    if (isMassInt(item))
    {
        return stoi(item);
    }

    if (calculatedMasses.count(item))
    {
        return calculatedMasses[item];
    }

    auto& currentNode = scaleNamesToNodes[item];
    int totalMass = 1 + calculateTotalMass(currentNode.left) + calculateTotalMass(currentNode.right);

    calculatedMasses[item] = totalMass;
    return totalMass;
}


/**
 * @brief Recursively evaluates the balancing requirements for a given item
 * and stores the results in the imbalanceFixes map.
 *
 * @param item Mass or scale name.
 */
void measureImbalances(const string& item) {
    if (isMassInt(item)) {
        return;
    }

    auto& currentNode = scaleNamesToNodes[item];
    int leftMass = 0;
    int rightMass = 0;

    // Retrieve the (recursively calculated) masses of the left pan
    if (isMassInt(currentNode.left))
    {
        leftMass = stoi(currentNode.left);
    }
    else
    {
        leftMass = calculatedMasses[currentNode.left];
    }

    // Retrieve the (recursively calculated) masses of the right pan
    if (isMassInt(currentNode.right))
    {
        rightMass = stoi(currentNode.right);
    }
    else
    {
        rightMass = calculatedMasses[currentNode.right];
    }

    // Measure imbalances
    int leftAdditionalWeight = 0;
    int rightAdditionalWeight = 0;

    if (leftMass > rightMass) {
        leftAdditionalWeight = leftMass - rightMass;
    }
    else if (rightMass > leftMass) {
        rightAdditionalWeight = rightMass - leftMass;
    }

    imbalanceFixes[item] = make_pair(leftAdditionalWeight, rightAdditionalWeight);

    measureImbalances(currentNode.left);
    measureImbalances(currentNode.right);
}

int main() {
    std::ifstream input("input\\input.txt");
    if (!input) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return 1;
    }
    std::cin.rdbuf(input.rdbuf());

    string line;

    while (getline(cin, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string name, left, right;

        getline(ss, name, ',');
        getline(ss, left, ',');
        getline(ss, right, ',');

        if (name.empty() || left.empty() || right.empty()) {
            cerr << "Malformed input line skipped: " << line << endl;
            continue;
        }

        scaleNames.push_back(name);
        scaleNamesToNodes[name] = { left, right };
    }

    for (const string& name : scaleNames) {
        calculateTotalMass(name);
    }

    for (const string& name : scaleNames) {
        measureImbalances(name);
    }

    for (const string& name : scaleNames) {
        int leftImbalanceCorrection = imbalanceFixes[name].first;
        int rightImbalanceCorrection = imbalanceFixes[name].second;
        cout << name << "," << leftImbalanceCorrection << "," << rightImbalanceCorrection << endl;
    }

    return 0;
}
