#include <string>
#include <vector>
#include "Appointment.h"
#ifndef USER_CLASS
#define USER_CLASS
class User{
	
private:
	std::string username;
	std::string email;
	std::string passwd;
	std::string phone;
	std::string name;
	std::vector<Appointment> appointments;

public:
	User(std::string user, std::string email, std::string passwd, std::string phone, std::string nameUse);
	User();
	bool update(std::string field, std::string item);
	std::string getUsername();
	std::string getPasswd();
	void addAppointment(Appointment appoint);
	void loadAppointments();
	std::vector<Appointment> getAppointments();
	bool deleteAppointment(int index);
	std::string getCsvString();
	void updateAppointmentList();
	bool updateAppointment(int index, std::string field, std::string change);
};

#endif //USER_CLASS