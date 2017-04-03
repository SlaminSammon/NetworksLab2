#include "Appointment.h"
#include <string>
#include "stdio.h"

Appointment::Appointment(std::string dayte, std::string thyme, std::string tytle){
	date = dayte;
	time = thyme;
	title = tytle;
}
Appointment::Appointment(){
	date = "Date";
	time = "Time";
	title = "Title";
}
bool Appointment::update(std::string query, std::string item){
	if(tolower(query[0]) == 'a'){
		title = item;
		return true;
	}
	else if(tolower(query[0]) == 'b'){
		time = item;
		return true;
	}
	else if(tolower(query[0]) == 'c'){
		date = item;
		return true;
	}
	else return false;
}
std::string Appointment::getTitle(){
	return title;
}
std::string Appointment::getDate(){
	return date;
}
std::string Appointment::getTyme(){
	return time;
}
std::string Appointment::sendCSVString(){
	return date + "," + time + "," + title + ',';
}