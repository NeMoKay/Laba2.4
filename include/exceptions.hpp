#pragma once

#include <string>
#include <iostream>
#include <cstdlib>


class Exception{
private:
    std::string error;
public:
    Exception(const char* message){
        if (message == nullptr){
            error = "";
        } 
        else{
            error = message;
        }
    }

    Exception(const std::string& message){
        error = message;
    }

    const char* what() const noexcept{
        return error.c_str();
    }
};

class IndexOutOfRangeException : public Exception{
public:
    IndexOutOfRangeException(const char* message) : Exception(message)  {}
    IndexOutOfRangeException(const std::string& message) : Exception(message)  {}
};

class EmptySequenceException : public Exception{
public:
    EmptySequenceException(const char* message) : Exception(message)  {}
    EmptySequenceException(const std::string& message) : Exception(message)  {}
};

class InvalidSizeException : public Exception{
public:
    InvalidSizeException(const char* message) : Exception(message)  {}
    InvalidSizeException(const std::string& message) : Exception(message)  {}
};

class NullPtrException : public Exception{
public:
    NullPtrException(const char* message) : Exception(message) {}
    NullPtrException(const std::string& message) : Exception(message) {}
};

class LengthMismatchException : public Exception{
public:
    LengthMismatchException(const char* message) : Exception(message) {}
    LengthMismatchException(const std::string& message) : Exception(message)  {}
};


inline void Terminate(){
    try{
        throw;
    }
    catch (const Exception& error){
        std::cerr << std::endl << "[ОШИБКА] " << error.what() <<  std::endl;
    }
    std::abort();
}


class AutoExceptionOn{
public:
    AutoExceptionOn(){
        std::set_terminate(Terminate);
    }
};

inline AutoExceptionOn AutoHandlerInstance;