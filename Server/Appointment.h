#include <string>

#ifndef APPOINTMENT_CLASS
#define APPOINTMENT_CLASS
class Appointment{

private:
	std::string date;
	std::string time;
	std::string title;

public:
	Appointment(std::string dayte, std::string thyme, std::string tytle);
	Appointment();
	bool update(std::string query, std::string item);
	std::string getTitle();
	std::string getDate();
	std::string getTyme();
	std::string sendCSVString();
};

#endif //APPOINTMENT_CLASS