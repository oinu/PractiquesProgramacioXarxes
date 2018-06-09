#include "BBDD.h"



BBDD::BBDD(std::string ip,std::string user, std::string pwd)
{
	driver = get_driver_instance();
	con = driver->connect(ip.c_str(), user.c_str(), pwd.c_str());
	stmt = con->createStatement();
	srand(time(NULL));
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

bool BBDD::InsertSession(int idUser)
{
	std::string s = "INSERT INTO Sessions(ID_USER,NUM_GAMES) VALUES('" + std::to_string(idUser) + "',0)";
	stmt->execute(s.c_str());
	return true;
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

void BBDD::UpdateSessionEnd(int idUser)
{
	std::string s ="SELECT ID FROM Sessions WHERE ID_USER=" + std::to_string(idUser) + " ORDER BY ID DESC LIMIT 1";
	resulset=stmt->executeQuery(s.c_str());
	resulset->next();
	int id = resulset->getInt("ID");

	s = "UPDATE Sessions SET END = CURRENT_TIMESTAMP WHERE ID = " + std::to_string(id);
	stmt->execute(s.c_str());

}

void BBDD::UpdateSessionNumGames(int idUser)
{
	std::string s = "SELECT ID,NUM_GAMES FROM Sessions WHERE ID_USER=" + std::to_string(idUser) + " ORDER BY ID DESC LIMIT 1";
	resulset = stmt->executeQuery(s.c_str());
	resulset->next();
	int id = resulset->getInt("ID");
	int numGames = resulset->getInt("NUM_GAMES");
	numGames++;

	s = "UPDATE Sessions SET NUM_GAMES = "+std::to_string(numGames)+" WHERE ID = " + std::to_string(id);
	stmt->execute(s.c_str());

}

void BBDD::ReturnQuestion(std::string& question, std::string& ansA, std::string& ansB, std::string& ansC, std::string& ansD, int& correct, std::vector<int>& idQuestions)
{
	std::string s = "SELECT count(*) FROM Questions";
	resulset = stmt->executeQuery(s.c_str());
	resulset->next();
	
	//Cogemos el numero total de preguntas
	int max = resulset->getInt(1);

	//Escogemos una id al azar
	int randomId = rand() % max + 1;
	int i = 0;
	bool exist = false;

	//Comprovamos que no exista en la lista
	while (!exist && i<idQuestions.size())
	{
		exist=idQuestions[i] == randomId;
		i++;
	}

	//Si existe
	if (exist)
	{
		bool newNumber = false;

		//Buscamos un numero valido
		while (!newNumber)
		{
			//Generamos una id nueva
			randomId = rand() % max + 1;

			//Suponemos que no existe
			exist = false;

			//Comprovamos que no existe
			while (!exist && i<idQuestions.size())
			{
				exist = idQuestions[i] == randomId;
				i++;
			}

			//Si a recorrido toda la lista y no existe el numero, hemos encontrado una id valida
			newNumber = (i == idQuestions.size() && !exist);
		}

	}

	s = "SELECT * FROM Questions WHERE ID_QUESTION="+std::to_string(randomId);
	resulset=stmt->executeQuery(s.c_str());
	resulset->next();

	question = resulset->getString("QUESTION").c_str();
	ansA = resulset->getString("ANSWER_A").c_str();
	ansB = resulset->getString("ANSWER_B").c_str();
	ansC = resulset->getString("ANSWER_C").c_str();
	ansD = resulset->getString("ANSWER_D").c_str();
	correct = resulset->getInt("CORRECT");

	idQuestions.push_back(randomId);
}
