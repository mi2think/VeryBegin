#pragma once

#include "mysql.h"
#include <iostream>

class DumpSql
{
public:
	DumpSql(const char* host, const char* user, const char* passwd, const char* dbname);
	~DumpSql();

	bool Init();

	void QueryAddress(const char* address, const char* dir = "") const;

	void QueryAddressCount(const char* address) const;

	void QueryAllAddressCount() const;

	void QueryAddressLog(const char* address) const;

	void QueryTopRankAddress(size_t top, bool toFile, const char* dir = "") const;

	void QueryEmptyAddress(const char* dir = "") const;

	void SetTableName(const char* table) { tableName_ = table; }

	void SetVersion(const char* version) { charversion_ = version; }

	std::string GetVersion() { return charversion_; }

	std::string GetTableName() const { return tableName_.empty() ? GetTableNameByTime() : tableName_; }

	void GetLastVersion();

	void ShowOp() const;

	size_t GetAddressDumpCount(const char* address) const;

	std::string GetTableNameByTime() const;
private:
	const char* host_;
	const char* user_;
	const char* passwd_;
	const char* dbname_;
	MYSQL*	mySql_;
	bool	isConnected_;
	std::string tableName_;
	std::string charversion_;
	std::string exeDir_;
};

