/*

MIT License

Copyright (c) 2021 Pascal Eberlein

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef LIBMODULEPP_MODULEPP_H
#define LIBMODULEPP_MODULEPP_H

#include <any>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <fcntl.h>
#include <dlfcn.h>
#include <filesystem>

#define F_CREATE(T) extern "C" Module* create() {return dynamic_cast<T*>(new T);}


class IModule {
protected:
    bool doRun = true;
    bool error = false;
    std::string errorString;

public:
    IModule() = default;
    ~IModule() = default;

    [[maybe_unused]] void start() {
        std::thread t([this](){
            run();
        });
    }

    virtual void run() {
        onStart();
        while(doRun) {
            work();
        }
        onStop();
    }

    virtual void work() {}
    virtual void onStart() {}
    virtual void onStop() {}

    [[maybe_unused]] virtual void stop() {
        doRun = false;
    }

    [[maybe_unused]] [[nodiscard]] bool hasError() const {
        return error;
    }

    [[maybe_unused]] [[nodiscard]] std::string getErrorString() const {
        return errorString;
    }

    [[maybe_unused]] [[nodiscard]] bool isRunning() const {
        return doRun;
    }
};

class Module : public IModule {
public:
    Module(): IModule(){};
};

using pModule = Module*;
typedef pModule create_t();
using tModuleVector = std::vector<pModule>;

class [[maybe_unused]] ModuleLoader {
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

    /*!
     * load all shared objects in a directory
     * @tparam T
     * @param path std::string
     * @param verbose bool
     * @return std::vector<T*>
     */
    template<typename T> static std::vector<T*> loadDirectory(const std::string& path, bool verbose=false) {
        std::vector<T*> r;
        for(const auto& e : std::filesystem::directory_iterator(path)) {
            const auto& p = e.path();
            if(e.is_regular_file() && p.has_extension() && p.extension() == ".so") {
                r.push_back(load<T>(p, verbose));
            }
        }
        return r;
    }

    /*!
     * load a module from a path, returns a Module pointer
     * @param path std::string, path to shared object
     * @param verbose bool, show errors
     * @return Module*
     */
    static pModule load(const std::string& path, bool verbose=false) {
        return load<Module>(path, verbose);
    }

    /*!
     * load all shared objects from a path
     * @param path
     * @param verbose
     * @return
     */
    static tModuleVector loadDirectory(const std::string& path, bool verbose=false) {
        return loadDirectory<Module>(path, verbose);
    }
};

#endif //LIBMODULEPP_MODULEPP_H
