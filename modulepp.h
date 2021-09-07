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
#include <utility>
#include <vector>
#include <chrono>
#include <fcntl.h>
#include <dlfcn.h>
#include <filesystem>

#define F_CREATE(T) extern "C" T* create() {return new T;}

class IModule {
 protected:
  uint32_t m_u32CycleTime_ms;
  bool m_bRun;
  std::string m_sError;
  std::thread* m_pThread;
  std::string m_sName = "IModule";

 public:
  IModule(): m_u32CycleTime_ms(500), m_bRun(true), m_sError(), m_pThread(nullptr) {}
  explicit IModule(std::string i_sName): m_u32CycleTime_ms(500), m_bRun(true), m_sError(), m_pThread(nullptr), m_sName(std::move(i_sName)) {}

  ~IModule() {
    delete m_pThread;
  };

  bool start() {
    if (m_pThread != nullptr) {
      return false;
    }
    m_pThread = new std::thread([this]() {
      run();
    });
    return true;
  }

  bool join() {
    if (m_pThread == nullptr) {
      return false;
    }
    m_pThread->join();
    return true;
  }

  virtual void run() {
    onStart();
    while (m_bRun) {
      work();
      std::this_thread::sleep_for(std::chrono::milliseconds(m_u32CycleTime_ms));
    }
    onStop();
  }

  virtual void work() {}
  virtual void onStart() {}
  virtual void onStop() {}

  virtual bool stop() {
    if (m_pThread == nullptr) {
      return false;
    }
    m_bRun = false;
    return true;
  }

  [[nodiscard]] bool hasError() const {
    return !m_sError.empty();
  }

  [[nodiscard]] std::string getError() const {
    return m_sError;
  }

  [[nodiscard]] bool isRunning() const {
    return m_bRun;
  }

  [[nodiscard]] uint32_t getCycleTime() const {
    return m_u32CycleTime_ms;
  }

  void setCycleTime(uint32_t i_u32CycleTime) {
    m_u32CycleTime_ms = i_u32CycleTime;
  }

  std::string getName() {
    return m_sName;
  }
};

class ModuleLoader {
 public:
  /*!
   * path should be the absolute path to the file
   * @tparam T
   * @param path
   * @param verbose
   * @return
   */
  template<typename T>
  static T* load(const std::string& path, bool verbose = false) {
    void* h = dlopen(std::filesystem::absolute(path).c_str(), RTLD_LAZY);
    if (!h) {
      if (verbose) {
        printf("dlopen error: %s\n", dlerror());
      }
      return nullptr;
    }
    typedef T* create_t();
    auto* c = (create_t*) dlsym(h, "create");
    auto e = dlerror();
    if (e) {
      if (verbose) {
        printf("dlsym error: %s\n", e);
      }
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
  template<typename T>
  static std::vector<T*> loadDirectory(const std::string& path, bool verbose = false) {
    std::vector<T*> r;
    for (const auto& e: std::filesystem::directory_iterator(path)) {
      const auto& p = e.path();
      if (e.is_regular_file() && p.has_extension() && p.extension() == ".so") {
        auto *ptr = load<T>(p, verbose);
        if(ptr != nullptr) {
          r.push_back(ptr);
        }
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
  template<typename ModuleType>
  static ModuleType* loadModule(const std::string& path, bool verbose = false) {
    return load<ModuleType>(path, verbose);
  }
};

#endif //LIBMODULEPP_MODULEPP_H
