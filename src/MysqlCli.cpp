/*
 * MysqlCli.cpp
 *
 *  Created on: Dec 29, 2016
 *      Author: xjxing
 */

#include "MysqlCli.h"
#include <iostream>
#include <string.h>
using namespace std;

MysqlCli::MysqlCli() :procresult(NULL),row(NULL){

}

MysqlCli::~MysqlCli() {
}

int MysqlCli::InitConnect(char* host,char* user,char* psw,char* server)
{
	if(mysql_init(&mysql) == NULL)
	{
		cout<<"open mysql fail!"<<endl;
		return -1;
	}

	else
	{
		cout<<"open mysql suc!\r\n";
		if (!mysql_real_connect(&mysql, host, user, psw, server, 0, NULL, 0)) {
	  		cout<<"mysql_real_connect fail!"<<endl;
			return -1;
			}
	    mysql_set_character_set(&mysql,"utf8");
	}

	return 0;
}

int MysqlCli::DisConnect()
{
	mysql_close(&mysql);

	return 0;
}

int MysqlCli::Query(char* pquerystr)
{
	mysql_real_query(&mysql,pquerystr,strlen(pquerystr));

	MYSQL_RES *procresult = mysql_store_result(&mysql);
	int fieldnums = mysql_field_count(&mysql);
	if(fieldnums != 0 && NULL == procresult) //应该有数据，但是返回null集，出错
	{
		cout << "mysql_real_query fail errcode=" << mysql_errno(&mysql) << " querystr="<<pquerystr<<endl;
	}
	else if(NULL != procresult) //返回有效数据
	{
		cout <<"mysql_real_query return " << mysql_num_rows(procresult) <<" rows data" << endl;
		MYSQL_ROW row;
		while((row=mysql_fetch_row(procresult)) != NULL)
		{
		}
		mysql_free_result(procresult);
		procresult = NULL;
	}
	else //查询命令不返回记录集
	{
		cout << "mysql_real_query resulte=" << mysql_errno(&mysql) << endl;
	}

	//mysql_real_escape_string() 写二进制数据
	//mysql_fetch_fields(procresult) 会返回每个字段的属性信息
	//mysql_fetch_array(procresult，array_type) 可以返回每个字段的关联名称，就是可以通过字段名称返回字段数据
	//mysql_insert_id() 会返回具有AUTO_INCREMENT索引的表插入时产生的最后一个id
	return 0;
}

int MysqlCli::BindRow(MYSQL_ROW row,void* datastruct)
{
	return 0;
}
