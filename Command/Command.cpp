#include <iostream>
#include <fstream>

#include <vector>

class LogCommand {
public:
    virtual ~LogCommand() = default;
    virtual void print(const std::string& message) = 0;
};

class ConsoleCommand : public LogCommand
{
public:
    ConsoleCommand(std::ostream& stream) : out(&stream) {}

    void print(const std::string& message) override
    {
        *out << message;
    }
    
private:
    std::ostream* out;
};

class FileCommand : public LogCommand 
{
public:
    FileCommand(const std::string& path) : path(path) {}

    void print(const std::string& message) override
    {
        std::ofstream outputFile(path);
        outputFile << message;
    }

private:
    std::string path;
};

void print(LogCommand* logComm)
{
    logComm->print("Command is everywhere!..I hope so...\n");
}


int main()
{
    ConsoleCommand consoleCommand(std::cout);
    FileCommand fileCommand("DataFile.txt");

    std::vector<LogCommand*> vecComm;
    vecComm.push_back(&consoleCommand);
    vecComm.push_back(&fileCommand);

    for (auto comm : vecComm)
    {
        print(comm);
    }

}