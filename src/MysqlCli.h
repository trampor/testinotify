/*
 * MysqlCli.h
 *
 *  Created on: Dec 29, 2016
 *      Author: xjxing
 */

#ifndef MYSQLCLI_H_
#define MYSQLCLI_H_

#include "mysql/mysql.h"

class MysqlCli {
public:
	MysqlCli();
	virtual ~MysqlCli();

	int InitConnect(char* host,char* user,char* psw,char* server);
	int DisConnect() ;

	int Query(char* pquerystr);
	//通过此函数实现不同表数据的获取
	virtual int BindRow(MYSQL_ROW row,void* datastruct);

private:
	MYSQL mysql;
    MYSQL_RES   *procresult;
    MYSQL_ROW   row;
};

#endif /* MYSQLCLI_H_ */
