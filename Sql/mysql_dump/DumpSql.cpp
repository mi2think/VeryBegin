#include "DumpSql.h"
#include "common.h"


DumpSql::DumpSql(const char* host, const char* user, const char* passwd, const char* dbname)
	: host_(host)
	, user_(user)
	, passwd_(passwd)
	, dbname_(dbname)
	, mySql_(NULL)
	, isConnected_(false)
{
	Init();
	char szDir[256] = { 0 };
	GetCurrentDirectory(sizeof(szDir), szDir);
	exeDir_ = szDir;
}

DumpSql::~DumpSql()
{
	if (isConnected_)
	{
		mysql_close(mySql_);
	}
}

bool DumpSql::Init()
{
	mySql_ = mysql_init(NULL);


	if (! mysql_real_connect(mySql_, host_, user_, passwd_, dbname_, 3306, NULL, 0))
	{
		return false;
	}

	isConnected_ = true;

	GetLastVersion();

	return true;
}

void DumpSql::QueryAddress(const char* address, const char* dir) const
{
	assert(address && dir);

	if (strlen(dir) == 0)
		dir = exeDir_.c_str();

	const char* sqlQuery = avar("SELECT * FROM `%s` WHERE address='%s' AND charversion='%s' ORDER By time DESC limit 1",
		GetTableNameByTime().c_str(), address, charversion_.c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });

		MYSQL_ROW  mySqlRow;
		if (mySqlRow = mysql_fetch_row(mySqlRes))
		{
			unsigned long* lengths = mysql_fetch_lengths(mySqlRes);
			std::ofstream of(avar("%s\\%s.log", dir, address));
			of << mySqlRow[5];

			std::ofstream data(avar("%s\\%s.dmp", dir, address), std::ios::out | std::ios::binary);
			data.write(mySqlRow[7], lengths[7]);
		}
	}
	else
	{
		std::cout << "error:" << mysql_error(mySql_) << "\n";
	}
}

void DumpSql::QueryAddressCount(const char* address) const
{
	size_t count = GetAddressDumpCount(address);
	std::cout << count << "\t" << address << "\n";
}

void DumpSql::QueryAllAddressCount() const
{
	const char* sqlQuery = avar("SELECT COUNT(*) FROM `%s` WHERE charversion='%s'", GetTableNameByTime().c_str(), charversion_.c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });

		MYSQL_ROW  mySqlRow;
		if (mySqlRow = mysql_fetch_row(mySqlRes))
		{
			std::cout << mySqlRow[0] << "\n";
		}
	}
	else
	{
		std::cout << "error:" << mysql_error(mySql_) << "\n";
	}
}

void DumpSql::QueryAddressLog(const char* address) const
{
	const char* sqlQuery = avar("SELECT * FROM `%s` WHERE address='%s' AND charversion='%s' ORDER By time DESC limit 1",
		GetTableNameByTime().c_str(), address, charversion_.c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });

		MYSQL_ROW  mySqlRow;
		if (mySqlRow = mysql_fetch_row(mySqlRes))
		{
			std::cout << mySqlRow[5] << "\n";
		}
	}
	else
	{
		std::cout << "error:" << mysql_error(mySql_) << "\n";
	}
}

void DumpSql::QueryTopRankAddress(size_t top, bool toFile, const char* dir) const
{
	struct AddrNum
	{
		size_t addr;
		size_t num;
	};

	size_t totalRows = 0;
	std::map<size_t, AddrNum> topRank;
	const char* sqlQuery = avar("SELECT address From %s WHERE charversion='%s'", GetTableNameByTime().c_str(), charversion_.c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });

		MYSQL_ROW mySqlRow;
		while (mySqlRow = mysql_fetch_row(mySqlRes))
		{
			const char* address = mySqlRow[0];
			size_t addr = 0;
			sscanf_s(address, "%x", &addr);
			topRank[addr].addr = addr;
			++topRank[addr].num;
			
			++totalRows;
		}
	}

	std::vector<const AddrNum*> sortTopRank;
	std::for_each(topRank.begin(), topRank.end(), [&](decltype(*topRank.begin())& e){ sortTopRank.push_back(&e.second); });
	std::sort(sortTopRank.begin(), sortTopRank.end(), [](const AddrNum* p1, const AddrNum* p2) { return p1->num > p2->num; } );
	
	std::cout << "address\t\tnum\n";


	size_t totalNum = 0;
	for (size_t i = 0; i < min(top, sortTopRank.size()); ++i)
	{
		const AddrNum* p = sortTopRank[i];
		std::string s = avar("0x%08x", p->addr);
		std::cout << s.c_str() << "\t" << p->num << "\n";
		totalNum += p->num;

		if (toFile)
		{
			QueryAddress(s.c_str(), dir);
		}
	}
}

void DumpSql::QueryEmptyAddress(const char* dir) const
{
	std::set<size_t> setEmptyAddr; 

	const char* sqlQuery = avar("SELECT address,log From %s WHERE charversion='%s'", GetTableNameByTime().c_str(), charversion_.c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });

		MYSQL_ROW mySqlRow;
		while (mySqlRow = mysql_fetch_row(mySqlRes))
		{
			const char* address = mySqlRow[0];
			size_t addr = 0;
			sscanf_s(address, "%x", &addr);

			auto it = setEmptyAddr.find(addr);
			if (it == setEmptyAddr.end())
			{
				const char* log = mySqlRow[1];
				const char* searchStr = "Address attemp to access ";
				const char* p = strstr(log, searchStr);
				if (p != NULL)
				{
					p += strlen(searchStr);
					std::string s(p, 10);

					size_t emptyAddr = 0;
					sscanf_s(s.c_str(), "%x", &emptyAddr);
					if (emptyAddr < 0xffff)
					{
						std::cout << s << "\n";
						setEmptyAddr.insert(addr);

						QueryAddress(address, dir);
					}
				}
			}
		}
	}
}

size_t DumpSql::GetAddressDumpCount(const char* address) const
{
	assert(address);

	size_t count = 0;
	const char* sqlQuery = avar("SELECT COUNT(*) FROM `%s` WHERE address='%s' AND charversion='%s'", 
		GetTableNameByTime().c_str(), address, charversion_.c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });
		MYSQL_ROW  mySqlRow;
		if (mySqlRow = mysql_fetch_row(mySqlRes))
			count = atoi(mySqlRow[0]);
	}
	else
	{
		std::cout << "error:" << mysql_error(mySql_) << "\n";
	}

	return count;
}

std::string DumpSql::GetTableNameByTime() const
{
	if (! tableName_.empty())
		return tableName_;

	time_t nowTime = time(NULL);
	tm newTime;
	errno_t err = localtime_s(&newTime, &nowTime);
	assert(err == 0);
	char sz[64] = { 0 };
	strftime(sz, sizeof(sz), "%Y%m%d", &newTime);
	return avar("ErrorDump%s", sz);
}

void DumpSql::GetLastVersion()
{
	const char* sqlQuery = avar("SELECT charversion FROM `%s` ORDER By time DESC limit 1", GetTableNameByTime().c_str());
	if (! mysql_real_query(mySql_, sqlQuery, strlen(sqlQuery)))
	{
		MYSQL_RES* mySqlRes = mysql_store_result(mySql_);
		ON_SCOPE_EXIT([&]{ mysql_free_result(mySqlRes); });

		MYSQL_ROW  mySqlRow;
		if (mySqlRow = mysql_fetch_row(mySqlRes))
		{
			charversion_ = mySqlRow[0] ;
		}
	}
	else
	{
		std::cout << "error:" << mysql_error(mySql_) << "\n";
	}
}

void DumpSql::ShowOp() const
{
	std::ifstream in("op.txt");
	std::cout << in.rdbuf();
}
