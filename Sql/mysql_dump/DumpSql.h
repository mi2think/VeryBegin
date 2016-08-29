#pragma once

#include "mysql.h"
#include <iostream>

class DumpSql
{
public:
	DumpSql(const char* host, const char* user, const char* passwd, const char* dbname);
	~DumpSql();

	bool Init();

	// 查询指定地址
	// 命令： query address dir
	// 例如： query 0x00c1e12d e:\dump\
	// 默认： query 0x00c1e12d ， 存入当前文件夹
	void QueryAddress(const char* address, const char* dir = "") const;

	// 查询指定地址宕机数
	// 命令： count address
	void QueryAddressCount(const char* address) const;

	// 查询总的宕机记录
	void QueryAllAddressCount() const;

	// 查询指定地址log
	// 命令： log address
	void QueryAddressLog(const char* address) const;

	// 查询前X名的宕机地址
	// 命令： top X
	// 存入文件： top X dir
	void QueryTopRankAddress(size_t top, bool toFile, const char* dir = "") const;

	// 查询空指针
	// 命令： query empty dir
	void QueryEmptyAddress(const char* dir = "") const;

	// 设置表名字
	// 命令： set ErrorDump20140106
	void SetTableName(const char* table) { tableName_ = table; }

	// 设置版本号
	// 命令： set version X
	void SetVersion(const char* version) { charversion_ = version; }

	std::string GetVersion() { return charversion_; }

	std::string GetTableName() const { return tableName_.empty() ? GetTableNameByTime() : tableName_; }

	// 取最后一条宕机记录的版本号为当前默认版本号
	void GetLastVersion();

	// 显示操作
	void ShowOp() const;

	// 地址宕机数量
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

