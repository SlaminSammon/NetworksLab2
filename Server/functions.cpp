#include "functions.h"
#include "User.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "Appointment.h"

bool passwordVerify(std::string passwd, std::vector<User> &users, int index){
	//for every user currently in the database.
	if(users[index].getPasswd().compare(passwd) == 0) return true;
	else return false;
}
int verifyUser(std::string username, std::vector<User> &users){
	int i = getUserIndex(username,users);
	std::cout << i << std::endl;
	if(i != -1) return i;
	return -1;
}
void readUserFile(std::vector<User> &users){
	std::ifstream file;
	std::vector<std::string> data (5,"");
	std::string in;
	users.clear();
	int j=0;
	file.open("users.csv");
	while(file.good()){
		for(int i =0; i<5;++i){
			getline(file, data[i], ',');
		}
		getline(file,in);
		std::cout << data[0] << " "  <<data[1] << "\n";
		User readUser (data[0],data[2],data[1],data[3],data[4]);
		users.push_back(readUser);
		users[j].loadAppointments();
		++j;
	}
	file.close();
}
bool deleteUser(std::string username, std::vector<User> &users){
	int i = getUserIndex(username, users);
	if(i != -1){
		users.erase(users.begin() + i);//should erase the proper elements
			return true;
	}
	return false;
}
bool updateUser(std::string username, std::string field, std::string input, std::vector<User> &users){
	int i = getUserIndex(username, users);
	if(i != -1) return users[i].update(field,input);
	return false;
}
int getUserIndex(std::string username, std::vector<User> &users){
	for(int i = 0; i < users.size();++i){
		if(users[i].getUsername() == username){
			return i;
		}
 	}
	return -1;
}
bool newAppointment(std::string username, std::vector<User> &users, std::string title, std::string date, std::string tyme){
	int i = getUserIndex(username, users);
	if(i == -1) return false;
	Appointment newb (date, tyme, title);
	users[i].addAppointment(newb);
	return true;
}
std::vector<Appointment> getAppointmentList(std::string username, std::vector<User> &users){
	int i = getUserIndex(username, users);
	return users[i].getAppointments();
}
bool remAppointment(std::string username, std::vector<User> &users, int appointIndex){
	int i = getUserIndex(username, users);
	return users[i].deleteAppointment(appointIndex);
}
std::string getAppointString(Appointment app){
	return "Title: " + app.getTitle() + "\nDate: " + app.getDate() + "\nTime:" + app.getTyme() + "\n\n";
}
void updateUserFile(std::vector<User> &users){
	std::ofstream myfile ("users.csv");
	std::string loader = "";
	if(myfile.is_open()){
		for(int i =0; i<users.size(); ++i){
			if(i != users.size()-1) myfile << users[i].getCsvString() + ",\n";
			else myfile << users[i].getCsvString()+ "\n";
		}
		myfile.close();
	}
	else{
		printf("Error in file\n");
	}
}
void updateAppointmentFile(std::string username, std::vector<User> &users){
	int i = getUserIndex(username, users);
	users[i].updateAppointmentList();
}
std::string conflictCheck(std::string username, std::vector<User> &users){
	int i = getUserIndex(username, users);
	std::string returnString = "";
	std::vector<Appointment> appoint = users[i].getAppointments();
	for(int i = 0; i<appoint.size(); ++i){
		for(int j = i+1; i<appoint.size();++j){
			if(appoint[i].getDate().compare(appoint[j].getDate()) == 0
				&& appoint[i].getTyme().compare(appoint[j].getTyme()) == 0){
				returnString += "Conflict: " + appoint[i].getTitle() + " and " + appoint[j].getTitle() + "\n";
			}
		}
	}
	return returnString;
}
bool appointmentUpdate(std::string username, std::vector<User> &users, int index, std::string field, std::string change){
	int i = getUserIndex(username, users);
	return users[i].updateAppointment(index, field, change);
}
std::string timeRange(std::string username, std::vector<User> &users, std::string dateStart, std::string dateEnd){
	int i = getUserIndex(username, users);
	std::string returnString = "";
	std::vector<Appointment> appoint = users[i].getAppointments();
	for(i = 0; i< appoint.size();++i){
		if (inTimeRange(appoint[i], dateStart, dateEnd)){
			returnString += appoint[i].getTitle() +"\n";
		}
	}
	return returnString;
}
bool inTimeRange(Appointment appoint, std::string dateStart,std::string dateEnd){
	std::string appointDate = appoint.getDate();
	if(appointDate >= dateStart && appointDate <= dateEnd) return true;
	else return false;
}
void loginListNull(){
	std::ofstream myfile ("login.txt");
	if(myfile.is_open()){
		myfile.close();
	}
	else{
		printf("Error in file\n");
	}
}
bool checkLogin(std::string username){
	std::ifstream file;
	std::vector<std::string> strings;
	std::string in ="";
	file.open("login.txt");
	//read in the login file
	while(file.good()){
		getline(file,in);
		strings.push_back(in);
	}
	// check to see if the user was already logged in
	for(int i =0; i< strings.size(); ++i){
		if(username == strings[i]) return false;
	}
	file.close();
	std::ofstream myfile ("login.txt");
	//if user wasn't logged in rewrite file adding this user.
	if(myfile.is_open()){
		for(int i = 0; i<strings.size(); ++i){
			myfile << strings[i] + "\n";
		}
		myfile << username;
		myfile.close();
	}
	else{
		printf("Error in file\n");
	}
	return true;
}
void removeUserLogin(std::string username){
	std::ifstream file;
	std::vector<std::string> strings;
	std::string in ="";
	file.open("login.txt");
	//read in the login file
	while(file.good()){
		getline(file,in);
		strings.push_back(in);
	}
	file.close();
	std::ofstream myfile ("login.txt");
	//rewrite file without user
	if(myfile.is_open()){
		for(int i = 0; i<strings.size(); ++i){
			if(strings[i] != username) myfile << strings[i] + "\n";
		}
		myfile.close();
	}
	else{
		printf("Error in file\n");
	}
}