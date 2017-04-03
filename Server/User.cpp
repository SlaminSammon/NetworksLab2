#include "User.h"
#include <string>
#include "stdio.h"
#include <fstream>


User::User(std::string user, std::string emailAdd, std::string password, std::string phoneNum, std::string nameUse){
	username = user;
	email = emailAdd;
	passwd = password;
	phone = phoneNum;
	name = nameUse;
}
User::User(){
	username = "USERNAME";
	email = "EMAIL";
	passwd = "password";
	phone = "PHONE";
	name = "Henry Dillman";
}

bool User::update(std::string field, std::string item){
	if(field.length() != 1) return false;
	if(tolower(field[0]) == 'a'){
		username = item;
		printf("%s\n", username.c_str());
		return true;
	}
	else if(tolower(field[0]) == 'c'){
		email = item;
		return true;
	}
	else if(tolower(field[0]) == 'b'){
		passwd = item;
		return true;
	}
	else if(tolower(field[0]) == 'd'){
		phone = item;
		return true;
	}
	else if(tolower(field[0]) == 'e'){
		name = item;
		return true;
	}
	else return false;
}
std::string User::getUsername(){
	return username;
}

std::string User::getPasswd(){
	return passwd;
}
void User::addAppointment(Appointment appoint){
	appointments.push_back(appoint);
}
void User::loadAppointments(){
	std::string filename = username + ".csv";
	std::ifstream file;
	std::vector<std::string> data (3,"");
	file.open(filename.c_str());
	while(file.good()){
		for(int i =0;i<3; ++i){
			getline(file, data[i], ',');
		}
		Appointment appoint (data[0],data[1],data[2]);
		appointments.push_back(appoint);
	}

}
std::vector<Appointment> User::getAppointments(){
	return appointments;
}
bool User::deleteAppointment(int index){
	if(index >= appointments.size()) return false;
	int size = appointments.size();
	printf("%d\n", size);
	appointments.erase(appointments.begin() + index);
	size = appointments.size();
	printf("%d\n", size);
	return true;
}
std::string User::getCsvString(){
	return username + "," + passwd + "," + email + "," + phone + "," + name;
}
void User::updateAppointmentList(){
	std::string file = username + ".csv";
	std::ofstream myfile (file.c_str());
	if(myfile.is_open()){
		for(int i =0; i<appointments.size(); ++i){
			myfile << appointments[i].sendCSVString();
		}
		myfile.close();
	}
	else{
		printf("Error in file\n");
	}
}
bool User::updateAppointment(int index, std::string field, std::string change){
	return appointments[index].update(field, change);
}
