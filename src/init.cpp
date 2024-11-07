#include "../include/init.hpp"
#include <vector>

vector<i32> init() {
  vector<i32> array;
  ifstream file;
  file.open("init.txt", ios::in);
  if (!file.is_open()) {
    cout << "init.txt not found, creating new file" << endl;
    ofstream new_file;
    new_file.open("init.txt", ios::out);
    new_file << "100\n1000\n1000\n4099\n"; // default values for sdl2
    new_file.close();
    file.open("init.txt", ios::in);
  }
  if (file.is_open()) {
    string line;
    while (getline(file, line)) {
      array.push_back(stoi(line));
    }
  }
  file.close();

  return array;
}
