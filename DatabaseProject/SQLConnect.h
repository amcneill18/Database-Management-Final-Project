//Final Project For CS374 Database Management Systems
//City Pool Schedule Manager
// Created by Andrew McNeill and Jason Pinales
#ifndef SQLCONNECT_H
#define SQLCONNECT_H

#include <iomanip>
#include <windows.h>
#include <sqlext.h>
#include <mbstring.h>
#include <stdio.h>
#include <string>
#include <vector>

#define MAX_DATA 100  
#define MYSQLSUCCESS(rc) ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) ) 

template<typename T> void spacePrint(T t, const int& width) { // template function for outputing table
	std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}

class DB_EXE {
	struct ColDescription
	{
		SQLSMALLINT colNumber;
		SQLCHAR colName[80];
		SQLSMALLINT nameLen;
		SQLSMALLINT dataType;
		SQLULEN colSize;
		SQLSMALLINT decimalDigits;
		SQLSMALLINT nullable;
	};

public:
	SQLRETURN rc;	// ODBC return code  
	SQLHANDLE EnvHandle; // Environment     
	SQLHANDLE ConHandle; // Connection handle  
	SQLHANDLE StmtHandle; // Statement handle   
						  // Data source names
	SQLCHAR dbConnect[150] = "Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=E:\\DatabaseProject.accdb;UID=;PWD=";
	unsigned char dbConnect1[MAX_DATA];
	SQLCHAR szConnectOut[1024];
	SQLSMALLINT cchConnect;
	std::vector<ColDescription> cols; // vector filled with column titles
	std::vector< std::vector<std::string> > colData; // vector filled with columns' data
public:
	DB_EXE(); // Default constructor  
	void sqlconn(); // Allocate Handles 
	void sqlexec(unsigned char *); // Executes SQL statement  
	void sqldisconn(); // Disconnect Handles
	SQLRETURN GetResultset();
	void ColumnDetalies();
	void error_out(); // Displays errors  
private:
	_inline SQLRETURN Describe(ColDescription& c);
	SQLRETURN GetColData(int colnum, std::string& str);
};

DB_EXE::DB_EXE() {
	_mbscpy_s(dbConnect1, MAX_DATA, (const unsigned char *)"Database Project");
}

SQLRETURN DB_EXE::GetColData(int colnum, std::string& str) {
	SQLCHAR buf[255] = { 0 };
	if ((rc = SQLGetData(StmtHandle, colnum, SQL_CHAR, buf, sizeof(buf), NULL)) == SQL_SUCCESS)
		str = reinterpret_cast<char*>(buf);
	return rc;
}

SQLRETURN DB_EXE::GetResultset() {
	// Get all column description
	ColumnDetalies();
	colData.clear();
	while (SQLFetch(StmtHandle) == SQL_SUCCESS) {
		// vector of strings to hold the column data
		std::vector<std::string> col;
		std::string data;
		int i = 1; // column number
				   // Add data to col vector
		while (GetColData(i, data) == SQL_SUCCESS) {
			col.push_back(data);
			++i;
		}
		colData.push_back(col);
	}
	return SQL_SUCCESS;
}

SQLRETURN DB_EXE::Describe(ColDescription& c) {
	return SQLDescribeCol(StmtHandle, c.colNumber, c.colName, sizeof(c.colName), &c.nameLen,
		&c.dataType, &c.colSize, &c.decimalDigits, &c.nullable);
}
void DB_EXE::ColumnDetalies() {
	ColDescription c;
	c.colNumber = 1;
	cols.clear();
	while (Describe(c) == SQL_SUCCESS) {
		cols.push_back(c);
		++c.colNumber;
	}
}

// Allocate handles & connect to data source
void DB_EXE::sqlconn() {
	HWND desktopHandle = GetDesktopWindow();
	SQLAllocEnv(&EnvHandle);
	SQLAllocConnect(EnvHandle, &ConHandle);
	rc = SQLDriverConnect(ConHandle, desktopHandle, (SQLCHAR*)TEXT(dbConnect),
		SQL_NTS, szConnectOut, 1024, &cchConnect, SQL_DRIVER_NOPROMPT);
	if (!MYSQLSUCCESS(rc)) {  //Error  
		error_out();
	}
	rc = SQLAllocStmt(ConHandle, &StmtHandle);
}

// Execute SQL command 
void DB_EXE::sqlexec(unsigned char * cmdstr) {
	SQLRETURN rc = SQL_SUCCESS;
	if (!MYSQLSUCCESS(rc)) {  //Error  
		error_out();
	}
	else {
		if (rc == SQL_SUCCESS) {//Prepare And Execute The SQL Statement
			rc = SQLExecDirect(StmtHandle, cmdstr, SQL_NTS);
			GetResultset();
			for (int i = 0; i < cols.size(); i++) { // Print Column Titles
				spacePrint(cols.at(i).colName, 19);
			}
			std::cout << std::endl;
			for (int n = 0; n < colData.size(); n++) { // Print Column Data
				for (int m = 0; m < cols.size(); m++) {
					spacePrint(colData.at(n).at(m), 19);
				}
				std::cout << std::endl;
			}
		}
	}
}

// Free/Deallocate handles and disconnect.
void DB_EXE::sqldisconn() {
	SQLFreeStmt(StmtHandle, SQL_DROP);
	SQLDisconnect(ConHandle);
	SQLFreeConnect(ConHandle);
	SQLFreeEnv(EnvHandle);
}

// Display error message
void DB_EXE::error_out() {
	sqldisconn();
	SQLCHAR state[255];
	SQLCHAR error[255];
	SQLINTEGER code;
	SQLSMALLINT cb;
	SQLError(EnvHandle, ConHandle, NULL, state, &code, error, 255, &cb);
	std::wcout << error << std::endl;
	exit(0);
}

#endif 