#include <string>
#include "User.h"
#include <vector>
#include "Appointment.h"
#ifndef LAB2_FUNCTIONS
#define LAB2_FUNCTIONS

bool passwordVerify(std::string passwd, std::vector<User> &users, int index);
int verifyUser(std::string username, std::vector<User> &users);
void readUserFile(std::vector<User> &users);
bool deleteUser(std::string username, std::vector<User> &users);
bool updateUser(std::string username, std::string field, std::string input, std::vector<User> &users);
int getUserIndex(std::string username, std::vector<User> &users);
bool newAppointment(std::string username, std::vector<User> &users, std::string title, std::string date, std::string tyme);
std::vector<Appointment> getAppointmentList(std::string username, std::vector<User> &users);
bool remAppointment(std::string username, std::vector<User> &users, int appointIndex);
std::string getAppointString(Appointment app);
void updateUserFile(std::vector<User> &users);
void updateAppointmentFile(std::string username, std::vector<User> &users);
std::string conflictCheck(std::string username, std::vector<User> &users);
bool appointmentUpdate(std::string username, std::vector<User> &users, int index, std::string field, std::string change);
std::string timeRange(std::string username, std::vector<User> &users, std::string dateStart, std::string dateEnd);
bool inTimeRange(Appointment appoint, std::string dateStart,std::string dateEnd);

#endif //LAB2_FUNCTIONS