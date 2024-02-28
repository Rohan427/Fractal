#pragma once

#include "Constants.h"

class ErrorHandler
{
	int m_status = {ERR_SUCCESS};
	std::string m_msg = {ERR_OK};
	std::string m_detail = {"/n"};
	int rendStatus = REND_OK;

	public:
	void setErrorData (int status, std::string msg, std::string detail, int lrendStatus);
	int getStatus ();
	std::string getMsg ();
	std::string getDetail ();
	void setRendStatus (int status);
	int getRenderStatus ();
};
