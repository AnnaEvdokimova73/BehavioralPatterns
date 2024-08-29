#include <iostream>
#include <fstream>

#include <vector>

class Observer 
{
public:
    virtual ~Observer() = default;
    virtual void onWarning(const std::string& message) {}
    virtual void onError(const std::string& message) {}
    virtual void onFatalError(const std::string& message) {}
};

class ConsoleObserver : public Observer
{
public:
    ConsoleObserver(std::ostream& stream) : out(&stream) {}

    void onWarning(const std::string& message) override
    {
        *out << message;
    }

private:
    std::ostream* out;
};

class FileObserver : public Observer
{
public:
    FileObserver(const std::string& path) : path(path) {}

    void onError(const std::string& message) override
    {
        std::ofstream outputFile(path);
        outputFile << message;
    }

private:
    std::string path;
};

class FatalObserver : public Observer
{
public:
    FatalObserver(std::ostream& stream, const std::string& path) : out(&stream), path(path) {}

    void onFatalError(const std::string& message) override
    {
        *out << message;

        std::ofstream outputFile(path);
        outputFile << message;
    }

private:
    std::string path;
    std::ostream* out;
};

class DataForObserving
{
public:
    void addObserver(Observer* obs)
    {
        observers.push_back(obs);
    }

    void removeObserver(Observer* obs)
    {
        auto it = std::remove(observers.begin(), observers.end(), obs);
        observers.erase(it, observers.end());
    }

    void warning(const std::string& message) const
    {
        for (Observer* obs : observers)
        {
            obs->onWarning(message);
        }
    }
    void error(const std::string& message) const
    {
        for (Observer* obs : observers)
        {
            obs->onError(message);
        }
    }

    void fatalError(const std::string& message) const
    {
        for (Observer* obs : observers)
        {
            obs->onFatalError(message);
        }
    }


private:
    std::vector<Observer*> observers;
};

int main()
{
    ConsoleObserver consoleObserver(std::cout);
    FileObserver fileObserver("DataFile.txt");
    FatalObserver fatalObserver(std::cout, "DataFile.txt");

    DataForObserving dataForObserving;
    dataForObserving.addObserver(&consoleObserver);
    dataForObserving.addObserver(&fileObserver);
    dataForObserving.addObserver(&fatalObserver);

    dataForObserving.warning("My Warning!");
    //dataForObserving.error("My Error!");
    //dataForObserving.fatalError("My Fatal Error!");

}