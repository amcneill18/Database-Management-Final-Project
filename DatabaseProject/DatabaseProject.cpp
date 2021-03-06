#include <iostream>
#include <string>
#include <fstream>
#include "SQLConnect.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include <sqlucode.h> 
#include <odbcinst.h>

#undef max

int menu(), menu2();
void get_Sch();
void get_Blocks();
void get_Workers(std::string a);
void sch_Edit(std::string b);
void get_Emp();
std::string Emp_edit_menu();
void Emp_edit(std::string c);
void optimizer();
void exit();

int main() {
	system("title CS374 -- Schedule Optimizer Database Program");
	system("pause");
	system("color F0"); // background = 0 = black, text = F = white
	system("cls");
	srand(time(NULL));

	while (int input = menu()) {
		switch (input) {
		case 1:
			get_Sch();
			break;
		case 2:
			get_Emp();
			break;
		case 3:
			optimizer();
			break;
		case 4:
			exit();
			break;
		default:
			std::cout << "Invalid input, please try again" << std::endl;
			std::cin.clear();
			std::cin.ignore(MAXINT, '\n');
			std::cout << std::endl;
			system("pause");
			system("cls");
			continue;
		}
		system("pause");
		system("cls");
		continue;
	}
	system("cls");
	return 0;
}

int menu() {
	int choice;
	std::cout << "1. Schedule menu" << std::endl;
	std::cout << "2. Employee menu" << std::endl;
	std::cout << "3. Optomizer menu" << std::endl;
	std::cout << "4. Exit" << std::endl;
	std::cin >> choice;
	system("cls");
	return choice;
};

void get_Sch() {
	DB_EXE x;
	x.sqlconn();   // Allocate handles, and connect.	
	std::cout << "Schedules available for: \n";
	x.sqlexec((UCHAR FAR *)"SELECT Format(S.[Week of], 'Short Date') AS [Week of] FROM Schedule AS S");   // Execute SQL command
	std::string choice;
	std::cout << "Would you like to view a given week's schedule?: (y = yes; n = no) ";
	std::cin >> choice;
	while (choice != "y" && choice != "n") {
		std::cin.clear();
		system("cls");
		std::cout << "Invalid output, please try again\n";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		get_Sch();
	};
	if (choice == "y") {
		get_Blocks();
	}
}

void get_Blocks() {
	std::cout << "Please select a starting scheduale date: (mm/dd/yyyy) ";
	std::string choice2, choice3, choice4;
	std::cin >> choice2;
	system("cls");
	DB_EXE x;   // Declare an instance of the direxec object.
	x.sqlconn();   // Allocate handles, and connect.
	std::string input1 = "SELECT B.Title AS Shift, E.[First Name] + ' ' + E.[Last Name] AS [Head Guard], ";
	input1 += "B.[Guards Required], B.[Aids Required] FROM Block AS B, Employee AS E WHERE B.[Week of] = #";
	input1 += choice2;
	input1 += "# AND B.[Head Guard ID] = E.[Employee ID]";
	x.sqlexec((unsigned char *)input1.c_str());   // Execute SQL command  
	std::cout << std::endl;
	get_Workers(choice2);
	std::cout << "Edit Schedule? (y = yes; n = no) ";
	std::cin >> choice3;
	while (choice3 != "y" && choice3 != "n") {
		std::cin.clear();
		system("cls");
		std::cout << "Invalid output, please try again\n";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		get_Emp();
	};
	if (choice3 == "y") {
		std::cout << "Would you like to add or remove an employee from the scheduale? (add = a; remove = r) ";
		std::cin >> choice4;
		while (choice4 != "a" && choice4 != "r") {
			std::cin.clear();
			system("cls");
			std::cout << "Invalid output, please try again\n";
			std::cout << "Would you like to add or remove an employee from the scheduale? (add = a; remove = r) ";
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> choice4;
		};
		sch_Edit(choice4);
	}
}

void get_Workers(std::string a) {
	DB_EXE x;
	x.sqlconn();   // Allocate handles, and connect.
	std::string workers_query = "SELECT E.[First Name] + ' ' + E.[Last Name] AS Name, B.Title AS Shift ";
	workers_query += "FROM Block AS B, Employee AS E, [Hour Log] AS HL ";
	workers_query += "WHERE B.[Week of] = #";
	workers_query += a;
	workers_query += "# AND B.[Block ID] = HL.[Block ID] AND E.[Employee ID] = HL.[Employee ID]";
	std::cout << "Employees Scheduled \n\n";
	x.sqlexec((UCHAR FAR *)workers_query.c_str());   // Execute SQL command
	x.sqldisconn(); // Free handles and disconnect
	std::cout << std::endl;
};

void sch_Edit(std::string b) {
	DB_EXE x;
	x.sqlconn();   // Allocate handles, and connect.
	int eID, bID;
	if (b == "a") {
		std::cout << "Enter Employee's ID Number: ";
		std::cin >> eID;
		std::cout << "Enter Block ID Number: ";
		std::cin >> bID;
		std::string sch_query = "INSERT INTO [Hour Log] VALUES(";
		sch_query += std::to_string(eID); sch_query += ", ";
		sch_query += std::to_string(bID); sch_query += ", ";
		sch_query += std::to_string(0); sch_query += ")";
		x.sqlexec((UCHAR FAR *)sch_query.c_str());   // Execute SQL command
		sch_query.clear();
		x.sqldisconn(); // Free handles and disconnect

		x.sqlconn();   // Allocate handles, and connect.
					   // update work hours 
		sch_query = "UPDATE FROM [Hour Log] SET [Hours Worked] = Block.Hours WHERE [Hour Long].[Block ID] = Block.[Block ID]";
		sch_query = "UPDATE FROM BLOCKS SET [Guards Requred] - 1";
		x.sqlexec((UCHAR FAR *)sch_query.c_str());
		x.sqldisconn(); // Free handles and disconnect
	}
	else {
		std::cout << "Enter Employee's ID Number: ";
		std::cin >> eID;
		std::cout << "Enter Block ID Number: ";
		std::cin >> bID;
		x.sqlconn();   // Allocate handles, and connect.
		std::string sch_query = "DELETE FROM [Hour Log] WHERE [Employee ID] = ";
		sch_query += std::to_string(eID); sch_query += " AND [Block ID] = "; sch_query += std::to_string(bID);
		x.sqlexec((UCHAR FAR *)sch_query.c_str());
		x.sqldisconn(); // Free handles and disconnect
	}
}

void get_Emp() {
	DB_EXE x;
	x.sqlconn();   // Allocate handles, and connect.
	std::string s;
	std::cout << "Employees --\n";
	x.sqlexec((UCHAR FAR *)"SELECT * FROM Employee");   // Execute SQL command
	std::cout << "\nWould you like to edit the Employee list? (y = yes; n = no) ";
	std::cin >> s;
	while (s != "y" && s != "n") {
		std::cin.clear();
		system("cls");
		std::cout << "Invalid output, please try again\n";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		get_Emp();
	};
	if (s == "y") {
		x.sqldisconn(); // Free handles and disconnect
		Emp_edit(Emp_edit_menu());
	}
};

std::string Emp_edit_menu() {
	std::string i;
	while (int input = menu2()) {
		switch (input) {
		case 1:
			i = "a";
			break;
		case 2:
			i = "d";
			break;
		case 3:
			exit();
			break;
		default:
			std::cout << "Invalid input, please try again" << std::endl;
			std::cin.clear();
			std::cin.ignore(MAXINT, '\n');
			std::cout << std::endl;
			system("pause");
			system("cls");
			continue;
		}
		return i;
	}
}

void Emp_edit(std::string b) {
	std::string c, firstname, lastname;
	int EmpID;
	char sex, cert;
	if (b == "a") {
		DB_EXE x;
		x.sqlconn();   // Allocate handles, and connect.
		std::cout << "Enter Employee's first name: ";
		std::cin >> firstname;
		std::cout << "Last name: ";
		std::cin >> lastname;
		std::cout << "Sex: (M or F) ";
		std::cin >> sex;
		while (toupper(sex) != 'M' && toupper(sex) != 'F') {
			std::cin.clear();
			system("cls");
			std::cout << "Invalid output, please try again\n";
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			get_Emp();
		};
		std::cout << "Certifications: (Head Gaurd = H; Gaurd = G; Aid = A) ";
		std::cin >> cert;
		while (toupper(cert) != 'H' && toupper(cert) != 'G' && toupper(cert) != 'A') {
			std::cin.clear();
			system("cls");
			std::cout << "Invalid output, please try again\n";
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			get_Emp();
		};
		// random numbers for new SSN and employee ID
		double randSSN = rand() % 99999999 + 100101000;
		int randID = rand() % 999 + 111;
		c = "INSERT INTO Employee VALUES (";
		c += std::to_string(randSSN);
		c += ", "; c += std::to_string(randID);
		c += ", '"; c += firstname; c += "' , '"; c += lastname; c += "' , '"; c += sex; c += "')";
		x.sqlexec((UCHAR FAR *)c.c_str());
		c.clear();
		x.sqlexec((UCHAR FAR *)"SELECT * FROM Employee");// Execute SQL command
		x.sqldisconn(); // Free handles and disconnect

		x.sqlconn();   // Allocate handles, and connect.
		c = "INSERT INTO Certifications VALUES ("; c += std::to_string(randID); c += ", ";
		if (toupper(cert) == 'H') { c += "1,1,1)"; }
		else if (toupper(cert) == 'G') { c += "0,1,1)"; }
		else { c += ",0,0,1)"; }
		x.sqlexec((UCHAR FAR *)c.c_str());
		x.sqlexec((UCHAR FAR *)"SELECT * FROM Certifications");
		c.clear();
		x.sqldisconn(); // Free handles and disconnect
		system("pause");
	}
	if (b == "d") {
		DB_EXE x;
		x.sqlconn();   // Allocate handles, and connect.
		std::cout << "Enter Employee's ID: ";
		std::cin >> EmpID;
		c.clear();
		c = "DELETE FROM Employee WHERE Employee.[Employee ID] = ";
		c += std::to_string(EmpID);
		x.sqlexec((UCHAR FAR *)c.c_str());
		c.clear();
		x.sqldisconn(); // Free handles and disconnect

		x.sqlconn();   // Allocate handles, and connect.
		c = "DELETE FROM Certifications WHERE Certifications.[Employee ID] = ";
		c += std::to_string(EmpID);
		x.sqlexec((UCHAR FAR *)c.c_str());
		x.sqlexec((UCHAR FAR *)"SELECT * FROM Employee");   // Execute SQL command
		x.sqldisconn(); // Free handles and disconnect
		system("pause");
	}
}

int menu2() {
	int choice;
	std::cout << "1. Add New Employee" << std::endl;
	std::cout << "2. Remove Employee" << std::endl;
	std::cout << "3. Exit" << std::endl;
	std::cin >> choice;
	return choice;
};

void optimizer() { // automatically fills empty spots on a schedule // not complete
	system("cls");
	std::cout << "GET OUT OF HERE!";
	system("pause");
};
void exit() {
	system("cls");
	std::cout << "Have a great day!" << std::endl;
	system("pause");
	exit(-1);
};