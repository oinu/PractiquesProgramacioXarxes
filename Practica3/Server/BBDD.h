#pragma once
#include <mysql_connection.h>
#include <cppconn\driver.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\exception.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

struct User {
	int id;
	std::string name;
	std::string password;
	std::string email;
};

class BBDD
{
private:
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::ResultSet* resulset;

public:
	BBDD(std::string ip, std::string user, std::string pwd);
	~BBDD();
	void SelectDataBase(std::string db);
	bool InsertUser(std::string name, std::string password, std::string email);
	bool ExistUser(std::string name);
	bool ExistEmail(std::string email);
	User ReturnUserByName(std::string name);
	User ReturnUserByEmail(std::string email);
	bool RestorePassword(std::string email,std::string pwd);
	void ChangeNameUser(std::string actual, std::string newName);
	void ChangeEmail(std::string name, std::string email);
	void ChangePassword(std::string email, std::string pwd);
	void ReturnQuestion(std::string& question, std::string& ansA, std::string& ansB, std::string& ansC, std::string& ansD, int& correct, std::vector<int>& idQuestions);
};

