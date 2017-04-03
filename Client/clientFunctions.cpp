#include "clientFunctions.h"
#include <string>
#include <sstream>
#include <iostream>

bool checkDateFormat(std::string date){
	std::string checker = "";
	if(date.length() != 8) return false;
	checker += date[0]; 
	checker += date[1];
	std::cout << checker +"\n";
	int month = stringToInt(checker);
	if(month < 1 || month > 12) return false;
	if(date[2] != '/'|| date[5] != '/') return false;
	checker = "";
	checker += date[3];
	checker += date[4];
	int day = stringToInt(checker);
	if(month == 1 || month == 3 || month == 5 || month == 7 || 
		month == 8 || month == 10 || month == 12){
		if(day <1 || day >31) return false;
	}
	else if(month == 4 || month == 6 || month == 9 || month == 11){
		if(day <1 || day >30) return false;
	}
	else if(month == 2){
		checker = "";
		checker += date[6];
		checker += date[7];
		int year = stringToInt(checker);
		if(year % 4 == 0){
			if(day <1 || day>29) return false;
		}
		else{
			if(day <1 || day>28) return false;
		}
	}
	else return false;
	return true;
}
int stringToInt(std::string str){
	int val = 0;
	std::istringstream (str) >> val;
	std::cout << val << std::endl;
	return val;
}
bool checkTimeFormat(std::string tyme){
	std::string checker = "";
	if(tyme.length() != 5) return false;
	checker += tyme[0];
	checker += tyme[1];
	int hour = stringToInt(checker);
	if(hour < 1 || hour > 12) return false;
	if(tyme[2] != ':') return false;
	checker = "";
	checker += tyme[0];
	checker += tyme[1];
	int minutes = stringToInt(checker);
	if(minutes < 0 || minutes > 60) return false;
	return true;
}