#include <iostream>
#include <fstream>

#include <vector>

enum ErrorType
{
    WARNING = 0,
    ERROR,
    FATAL_ERROR,
    UNKNOWN
};

class LogMessage
{
public:
    LogMessage(ErrorType errType, const std::string& mes) : errorType(errType), logMes(mes) {}

    ErrorType type() const
    {
        return errorType;
    }

    const std::string& message() const
    {
        return logMes;
    }

private:
    ErrorType errorType;
    std::string logMes;
};

class BaseError
{
public:
    virtual void handleRequest(LogMessage* logMessage) = 0;
    virtual void setNextHandler(BaseError* handler) = 0;
};

class WarnError : public BaseError
{
public:
    void setNextHandler(BaseError* handler) override
    {
        nextHandler = handler;
    }

    void handleRequest(LogMessage* logMessage) override
    {
        if (logMessage->type() == ErrorType::WARNING)
        {
            std::cout << logMessage->message() << std::endl;
        }
        else if (nextHandler != nullptr)
        {
            nextHandler->handleRequest(logMessage);
        }
    }
private:
    BaseError* nextHandler;
};

class UsualError : public BaseError
{
public:
    UsualError(const std::string& path) : path(path){}
    void setNextHandler(BaseError* handler) override
    {
        nextHandler = handler;
    }

    void handleRequest(LogMessage* logMessage) override
    {
        if (logMessage->type() == ErrorType::ERROR)
        {
            std::ofstream outputFile(path);
            outputFile << logMessage->message();
        }
        else if (nextHandler != nullptr)
        {
            nextHandler->handleRequest(logMessage);
        }
    }
private:
    BaseError* nextHandler;
    std::string path;
};

class FatalError : public BaseError
{
public:
    void setNextHandler(BaseError* handler) override
    {
        nextHandler = handler;
    }

    void handleRequest(LogMessage* logMessage) override
    {
        if (logMessage->type() == ErrorType::FATAL_ERROR)
        {
            std::string info = "Fatal error: " + logMessage->message();
            throw std::runtime_error(info);
        }
        else if (nextHandler != nullptr)
        {
            nextHandler->handleRequest(logMessage);
        }
    }
private:
    BaseError* nextHandler;
};

class UnknownError : public BaseError
{
public:
    void setNextHandler(BaseError* handler) override
    {
        nextHandler = handler;
    }

    void handleRequest(LogMessage* logMessage) override
    {
        if (logMessage->type() == ErrorType::UNKNOWN)
        {
            std::string info = "Unknown message: " + logMessage->message();
            throw std::runtime_error(info);
        }
        else if (nextHandler != nullptr)
        {
            nextHandler->handleRequest(logMessage);
        }
    }
private:
    BaseError* nextHandler;
};


int main()
{
    try
    {
        std::unique_ptr<LogMessage> fatalMes = std::make_unique<LogMessage>(FATAL_ERROR, "Everything is broken!");
        std::unique_ptr<LogMessage> errorMes = std::make_unique<LogMessage>(ERROR, "The most common mistake.");
        std::unique_ptr<LogMessage> warnMes = std::make_unique<LogMessage>(WARNING, "Just a warning");
        std::unique_ptr<LogMessage> unknownMes = std::make_unique<LogMessage>(UNKNOWN, "Don't know that");

        std::unique_ptr<BaseError> warnError = std::make_unique<WarnError>();
        std::unique_ptr<BaseError> usualError = std::make_unique<UsualError>("DataFile.txt");
        std::unique_ptr<BaseError> fatalError = std::make_unique<FatalError>();
        std::unique_ptr<BaseError> unknownError = std::make_unique<UnknownError>();

        fatalError->setNextHandler(usualError.get());
        usualError->setNextHandler(warnError.get());
        warnError->setNextHandler(unknownError.get());

        // fatalError->handleRequest(unknownMes);
        fatalError->handleRequest(warnMes.get());
        fatalError->handleRequest(errorMes.get());
        fatalError->handleRequest(fatalMes.get());
    }
    catch (const std::runtime_error& err)
    {
        std::cout << err.what();
    }
}