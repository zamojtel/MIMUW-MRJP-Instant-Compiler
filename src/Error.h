#ifndef ERROR_H
#define ERROR_H

class Error{
private:
    size_t m_line;
    std::string m_msg;
public:
    Error(size_t line,const std::string &msg);
    std::string to_string() const;
};
#endif