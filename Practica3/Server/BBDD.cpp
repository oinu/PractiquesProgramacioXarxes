#include "BBDD.h"



BBDD::BBDD(std::string ip,std::string user, std::string pwd)
{
	driver = get_driver_instance();
	con = driver->connect(ip.c_str(), user.c_str(), pwd.c_str());
	stmt = con->createStatement();
}


BBDD::~BBDD()
{
	delete stmt;
	delete con;
	delete resulset;
}

void BBDD::SelectDataBase(std::string db)
{
	std::string s = "USE " + db;
	stmt->execute(s.c_str());
}

bool BBDD::InsertUser(std::string name, std::string password, std::string email)
{
	if (ExistUser(name))
	{
		return false;
	}
	else if (ExistEmail(email))
	{
		return false;
	}
	else
	{
		std::string s = "INSERT INTO Users(Name,Password,Email) VALUES ('" + name + "','" + password + "','" + email + "')";
		stmt->execute(s.c_str());
		return true;
	}
}

bool BBDD::ExistUser(std::string name)
{
	std::string s = "SELECT * FROM Users WHERE Name='" + name+"'";
	resulset = stmt->executeQuery(s.c_str());
	return resulset->next();
}

bool BBDD::ExistEmail(std::string email)
{
	std::string s = "SELECT * FROM Users WHERE Email='" + email + "'";
	resulset = stmt->executeQuery(s.c_str());
	return resulset->next();
}

User BBDD::ReturnUserByName(std::string name)
{
	User u;
	u.password = "root";
	u.name = "root";
	if (ExistUser(name))
	{
		std::string s = "SELECT * FROM Users WHERE Name='" + name + "'";
		resulset = stmt->executeQuery(s.c_str());
		resulset->next();
		u.id = resulset->getInt("ID");
		u.name = resulset->getString("Name").c_str();
		u.password = resulset->getString("Password").c_str();
		u.email = resulset->getString("Email").c_str();
	}
	
	return u;
}

User BBDD::ReturnUserByEmail(std::string email)
{
	std::string s = "SELECT * FROM Users WHERE Email='" + email + "'";
	resulset = stmt->executeQuery(s.c_str());
	resulset->next();
	User u;
	u.id = resulset->getInt("ID");
	u.name = resulset->getString("Name").c_str();
	u.password = resulset->getString("Password").c_str();
	u.email = resulset->getString("Email").c_str();
	return u;
}

bool BBDD::RestorePassword(std::string email, std::string pwd)
{
	if (!ExistEmail(email))
	{
		return false;
	}
	else
	{
		ChangePassword(email, pwd);
		User u = ReturnUserByEmail(email);
		return true;
	}
}

void BBDD::ChangeNameUser(std::string actual, std::string newName)
{
	if (ExistUser(newName))
	{
		std::cout << " El nombre de usuario ya exite" << std::endl;
	}
	else
	{
		std::string s = "UPDATE Users SET Name ='" + newName + "' WHERE name='" + actual + "'";
		stmt->execute(s.c_str());
	}
}

void BBDD::ChangeEmail(std::string name, std::string email)
{
	std::string s = "UPDATE Users SET Email ='" + email + "' WHERE name='" + name+"'";
	stmt->execute(s.c_str());
}

void BBDD::ChangePassword(std::string email, std::string pwd)
{
	std::string s = "UPDATE Users SET Password ='" + pwd + "' WHERE Email='" + email + "'";
	stmt->execute(s.c_str());
}
