#include <iostream>
#include "sha256.h"
#include <vector>
#include <string>
#include <fstream>
 
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::find;

int loadPasswords(vector<string>& usernames, vector<string>& passwords) {
	ifstream loadFile("passwords.txt");
	if (!loadFile) {
    	return 0;
	}
	string tempLine;
	while (getline (loadFile, tempLine)) {
		int commaPosition = tempLine.find(',');
		usernames.push_back(tempLine.substr(0, commaPosition));
		passwords.push_back(tempLine.substr(commaPosition + 1, tempLine.length()));
	}
	loadFile.close();
	cout << "Passwords Loaded" << endl;
	return 1;
}

int writePasswords(vector<string>& usernames, vector<string>& passwords) {
	ofstream writeFile("passwords.txt", ofstream::trunc);
	for (int i = 0; i < usernames.size(); i++) {
		writeFile << usernames[i] << ',' << passwords[i];
		if (i != usernames.size() - 1) {
			writeFile << '\n';
		}
	}
	writeFile.close();
	return 1;
}

int validatePassword(string password) {
	int capitalLetters = 0;
	int lowercaseLetters = 0;
	int numbers = 0;
	int symbols = 0;
	for (int i = 0; i < password.length(); i++) {
		char temp = password[i];
		if (temp < 33) {
			return 0;
		}
		else if (temp < 48) {
			symbols++;
		}
		else if (temp < 58) {
			numbers++;
		}
		else if (temp < 65) {
			symbols++;
		}
		else if (temp < 91) {
			capitalLetters++;
		}
		else if (temp < 97) {
			symbols++;
		}
		else if (temp < 123) {
			lowercaseLetters++;
		}
		else if (temp < 127) {
			symbols++;
		}
	}
	
	if (capitalLetters > 0 && lowercaseLetters > 0 && numbers > 0 && symbols > 0 && password.length() >= 8) {
			return 1;
	}
	return 0;
}


int findIndex(string user, vector<string>& usernames) {
	int index = -1;
	for (int i = 0; i < usernames.size(); i++) {
		if (usernames[i] == user) {
			index = i;
			break;
		}
	}
	return index;
}

int createUser(vector<string>& usernames, vector<string>& passwords, string& user) {
	cout << "Please enter a username: ";
	string username;
	cin >> username;
	while (findIndex(username, usernames) > -1) {
		cout << "Username already taken.\nPlease enter a username: ";
		cin >> username;
	}
	user = username;
	cout << "Please enter a password: ";
	string password;
	cin >> password;
	while (validatePassword(password) == 0) {
		cout << "Passwords must be 8 characters long, have a lowercase letter, capital letter, symbol, and number.\nPlease enter a password: ";
		cin >> password;
	}
	password = sha256(password);
	usernames.push_back(username);
	passwords.push_back(password);
	writePasswords(usernames, passwords);
	return 1;
}

bool checkPassword(string user, string password, vector<string>& usernames, vector<string>& passwords) {
	int index = findIndex(user, usernames);
	if (index == -1) {
		return false;
	}
	if (passwords[index] == password) {
		return true;
	}
	return false;
}

int changePassword(string user, vector<string>& usernames, vector<string>& passwords) {
	cout << "Please enter your old password: ";
	string oldPassword;
	cin >> oldPassword;
	oldPassword = sha256(oldPassword);
	while (checkPassword(user, oldPassword, usernames, passwords) == false) {
		cout << "Password was incorrect.\nPlease enter your old password: ";
		cin >> oldPassword;
		oldPassword = sha256(oldPassword);
	}
	cout << "Please enter a new password: ";
	string newPassword;
	cin >> newPassword;
	while (validatePassword(newPassword) == 0) {
		cout << "Passwords must be 8 characters long, have a lowercase letter, capital letter, symbol, and number.\nPlease enter a password: ";
		cin >> newPassword;
	}
	newPassword = sha256(newPassword);
	int index = findIndex(user, usernames);
	passwords[index] = newPassword;
	writePasswords(usernames, passwords);
	return 1;
}

int main(int argc, char *argv[])
{
	vector<string> usernames;
	vector<string> passwords;
	
	loadPasswords(usernames, passwords);
	
	bool loggedIn = false;
	string user;
	string userResponse;
	
	while (true) {
		if (!loggedIn) {
			cout << "Hello! Please select an option (type the number):" << endl;
			cout << "1. Log In" << endl;
			cout << "2. Sign Up" << endl;
			cout << "3. Exit" << endl;
			cin >> userResponse;
			if (userResponse == "1") {
				if (usernames.size() == 0) {
					cout << "No existing users, please sign up!";
					continue;
				}
				cout << "Enter your username: ";
				cin >> user;
				if (findIndex(user, usernames) == -1) {
					cout << "User does not exist\n";
					continue;
				}
				else {
					cout << "Enter your password: ";
					string tempPassword;
					cin >> tempPassword;
					tempPassword = sha256(tempPassword);
					while (!checkPassword(user, tempPassword, usernames, passwords)) {
						cout << "Invalid password, please try again\n";
						cin >> tempPassword;
						tempPassword = sha256(tempPassword);
					}
					cout << "Logged in successfully!\n";
					loggedIn = true;
				}
			}
			else if (userResponse == "2") {
				if (createUser(usernames, passwords, user) == 1) {
					loggedIn = true;
					cout << "Account created successfully";
				}
			}
			else if (userResponse == "3") {
				cout << "Thanks for using my security system!";
				exit(0);
			}
		}
		else {
			cout << "Hello " << user << "! Please select an option (type the number):" << endl;
			cout << "1. Change Password" << endl;
			cout << "2. Sign Out" << endl;
			cout << "3. Exit" << endl;
			cin >> userResponse;
			if (userResponse == "1") {
				if (changePassword(user, usernames, passwords) == 1) {
					cout << "Password changed successfully\n";
				}
				else {
					cout << "Password change was unsuccessful";
				}
			}
			else if (userResponse == "2") {
				loggedIn = false;
				user = "";
				cout << "Logged out!";
			}
			else if (userResponse == "3") {
				cout << "Thanks for using my credential system!";
				exit(0);
			}
		}
	}	

    return 0;
}
