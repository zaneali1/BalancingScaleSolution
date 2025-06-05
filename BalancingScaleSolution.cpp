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
map<string, int>            calculatedMasses;
map<string, pair<int, int>> imbalanceFixes;

bool isMassInt(const string& item) {
    return all_of(item.begin(), item.end(), ::isdigit);
}

int calculateTotalMass(const string& item) {
    if (isMassInt(item)) {
        return stoi(item);
    }
    if (calculatedMasses.count(item)) {
        return calculatedMasses[item];
    }
    auto& currentNode = scaleNamesToNodes[item];
    int totalMass = 1 + calculateTotalMass(currentNode.left) + calculateTotalMass(currentNode.right);
    calculatedMasses[item] = totalMass;
    return totalMass;
}

void measureImbalances(const string& item) {
    if (isMassInt(item)) {
        return;
    }
    auto& currentNode = scaleNamesToNodes[item];
    int leftMass = isMassInt(currentNode.left) ? stoi(currentNode.left) : calculatedMasses[currentNode.left];
    int rightMass = isMassInt(currentNode.right) ? stoi(currentNode.right) : calculatedMasses[currentNode.right];

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
    ifstream input("input/input.txt");
    if (!input) {
        cerr << "Error: Could not open file!" << endl;
        return 1;
    }
    cin.rdbuf(input.rdbuf());

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
        cout << name << "," << imbalanceFixes[name].first << "," << imbalanceFixes[name].second << endl;
    }
    return 0;
}