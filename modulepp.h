#ifndef LIBMODULEPP_MODULEPP_H
#define LIBMODULEPP_MODULEPP_H

#include <any>
#include <map>
#include <string>
#include <thread>
#include <fcntl.h>
#include <dlfcn.h>

#define F_CREATE(T) extern "C" Module* create() {return dynamic_cast<T*>(new T);}


class IModule {
protected:
    bool doRun = true;

    bool error = false;
    std::string errorString;

public:
    IModule() = default;
    ~IModule() = default;

    void start() {
        std::thread t([this](){
            run();
        });
        t.detach();
    }

    void run() {
        while(doRun) work();
    }

    virtual void work() {}

    virtual void stop() {
        doRun = false;
    }

    bool hasError() const {
        return error;
    }

    std::string getErrorString() const {
        return errorString;
    }

    bool isRunning() const {
        return doRun;
    }
};

class Module : public IModule {
public:
    Module(): IModule(){};
};

typedef Module* pModule;
typedef pModule create_t();

class ModuleLoader {
public:
    /*!
     * path should be the absolute path to the file
     * @tparam T
     * @param path
     * @param verbose
     * @return
     */
    template<typename T> static T* load(const std::string& path, bool verbose=false) {
        void* h = dlopen(path.c_str(), RTLD_LAZY);
        if(!h) {
            if(verbose) printf("dlopen error: %s\n", dlerror());
            return nullptr;
        }
        auto* c = (create_t*) dlsym(h, "create");
        auto e = dlerror();
        if(e) {
            if(verbose) printf("dlsym error: %s\n", e);
            dlclose(h);
            return nullptr;
        }
        return c();
    }

    static pModule load(const std::string& path, bool verbose=false) {
        return load<Module>(path, verbose);
    }
};



#endif //LIBMODULEPP_MODULEPP_H
