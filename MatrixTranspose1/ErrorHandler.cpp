#include "ErrorHandler.h"
#include <iostream>

void ErrorHandler::setErrorData (int status, std::string msg, std::string detail, int lrendStatus = REND_OK)
{
	m_status = status;
	m_msg = msg;
	m_detail = detail;
	rendStatus = lrendStatus;
}

int ErrorHandler::getStatus ()
{
	return m_status;
}

std::string ErrorHandler::getMsg ()
{
	return m_msg;
}

std::string ErrorHandler::getDetail ()
{
	return m_detail;
}

void ErrorHandler::setRendStatus (int status)
{
	rendStatus = status;
}

int ErrorHandler::getRenderStatus ()
{
	return rendStatus;
}
