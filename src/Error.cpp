#include "Includes.h"

Error::Error(size_t line, const std::string &msg):m_line{line},m_msg{msg}{}

std::string Error::to_string() const{
    return std::format("Line: {} | Message: {}",m_line,m_msg);
}