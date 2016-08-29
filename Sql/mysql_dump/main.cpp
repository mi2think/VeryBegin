#include "DumpSql.h"
#include <iostream>
#include <string>

const char* host = "192.168.100.55";
const char* user = "errouser";
const char* passwd = "erropasswd";
const char* dbname = "GAMEERROR_HS";
const char* table = "ErrorDump20140106";

void Query(DumpSql& dumpSql, std::string& op)
{
	if (op == "query")
	{
		std::string address;
		std::cin >> address;

		std::string dir;
		std::cin >> dir;

		if (address == "empty")
		{
			dumpSql.QueryEmptyAddress(dir.c_str());
		}
		else
		{
			dumpSql.QueryAddress(address.c_str(), dir.c_str());
		}
	}
	else if (op == "count")
	{
		std::string address;
		std::cin >> address;

		if (address == "all")
		{
			std::cout << "总宕机记录数\n";
			dumpSql.QueryAllAddressCount();
		}
		else
		{
			std::cout << "该地址宕机记录数\n";
			dumpSql.QueryAddressCount(address.c_str());
		}
	}
	else if (op == "log")
	{
		std::string address;
		std::cin >> address;
		dumpSql.QueryAddressLog(address.c_str());
	}
	else if (op == "top")
	{
		std::string num;
		std::cin >> num;

		char ch;
		std::cin.get(ch);

		if (ch == '\n')
		{
			dumpSql.QueryTopRankAddress(atoi(num.c_str()), false);
		}
		else
		{
			std::string dir;
			std::cin >> dir;

			dumpSql.QueryTopRankAddress(atoi(num.c_str()), true, dir.c_str());
		}
	}
	else if (op == "set")
	{
		std::string tableName;
		std::cin >> tableName;

		if (tableName == "version")
		{
			std::string version;
			std::cin >> version;

			dumpSql.SetVersion(version.c_str());
			std::cout << "设置版本号成功 " << version.c_str() << "\n";
		}
		else
		{
			dumpSql.SetTableName(tableName.c_str());
			std::cout << "设置表名成功 " << tableName.c_str() << "\n";
		}
	}
}

int main()
{
	DumpSql dumpSql(host, user, passwd, dbname);
	dumpSql.ShowOp();
	std::cout <<">";

	std::string op;
	while (std::cin >> op)
	{
		Query(dumpSql, op);

		std::cout << "\n"<< dumpSql.GetTableName() << "\t" << dumpSql.GetVersion() << "\n";
		std::cout <<"************************************\n";
		std::cout <<">";
	}


	system("pause");

	return 0;
}

