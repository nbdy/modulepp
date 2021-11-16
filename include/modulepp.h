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

#define USE_OHLOG

#ifdef USE_OHLOG
#include "ohlog.h"
#endif

#define ENABLE_DRAW_FUNCTIONS

#include <any>
#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <chrono>
#include <fcntl.h>
#include <dlfcn.h>
#include <filesystem>
#include <sstream>
#include <condition_variable>

#define F_CREATE(T) extern "C" T* create() {return new T;}

using Path = std::filesystem::path;
using UniqueLock = std::unique_lock<std::mutex>;
using Clock = std::chrono::high_resolution_clock;
using Milliseconds = std::chrono::milliseconds;
using Nanoseconds = std::chrono::nanoseconds;

#define TIMESTAMP_MS std::chrono::duration_cast<Milliseconds>(Clock::now().time_since_epoch()).count()
#define TIMESTAMP_NS std::chrono::duration_cast<Nanoseconds>(Clock::now().time_since_epoch()).count()

class ModuleVersion {
  uint32_t m_u32Major = 0U;
  uint32_t m_u32Minor = 1U;
  uint32_t m_u32Patch = 0U;

public:
  ModuleVersion() = default;
  ModuleVersion(const ModuleVersion& other) {
    m_u32Major = other.m_u32Major;
    m_u32Minor = other.m_u32Minor;
    m_u32Patch = other.m_u32Patch;
  }
  ModuleVersion(uint32_t i_u32Major, uint32_t i_u32Minor, uint32_t i_u32Patch): m_u32Major(i_u32Major), m_u32Minor(i_u32Minor), m_u32Patch(i_u32Patch) {}

  [[nodiscard]] std::string toString() const {
    std::stringstream r;
    r << std::to_string(m_u32Major) << "." << std::to_string(m_u32Minor) << "." << std::to_string(m_u32Patch);
    return r.str();
  }
};

class ModuleInformation {
  ModuleVersion m_Version {0U, 1U, 0U};
  std::string m_sName;

public:
  ModuleInformation() = default;
  explicit ModuleInformation(std::string i_sName): m_sName(std::move(i_sName)) {};
  ModuleInformation(std::string i_sName, const ModuleVersion& i_Version): m_sName(std::move(i_sName)), m_Version(i_Version) {};

  [[nodiscard]] std::string toString() const {
    std::stringstream r;
    r << m_sName << " " << m_Version.toString();
    return r.str();
  };

  [[nodiscard]] ModuleVersion getVersion() const {
    return m_Version;
  };

  [[nodiscard]] std::string getName() const {
    return m_sName;
  }
};

class IModule {
 private:
  uint32_t m_u32CycleTime_ms = 500U;
  std::atomic_bool m_bRun = {true};
  std::atomic_bool m_bEnable = {false};
  std::atomic_bool m_bWorkTooExpensive = {false};
  std::string m_sError;
  std::thread m_Thread;
  std::mutex m_Mutex;
  std::condition_variable m_Condition;
  ModuleInformation m_Information;
  uint64_t m_u64FunctionStartTimestamp = 0;
  uint64_t m_u64FunctionEndTimestamp = 0;
  uint64_t m_u64FunctionTime = 0;
  uint32_t m_u32ModifiedInterval = 0;
  std::vector<ModuleInformation> m_Dependencies;
  std::map<std::string, IModule*> m_DependencyMap;

 public:
  IModule(): m_Thread([this] {run();}), m_Information() {};
  explicit IModule(ModuleInformation i_Information): m_Thread([this] {run();}), m_Information(std::move(i_Information)) {};
  IModule(ModuleInformation i_Information, std::vector<ModuleInformation> i_Dependencies): m_Thread([this] {run();}), m_Information(std::move(i_Information)), m_Dependencies(std::move(i_Dependencies)) {};

  ~IModule() {
    kill();
    join();
  }

  bool start() {
    if(m_bEnable) {
      return false;
    }
    m_bRun = true;
    m_bEnable = true;
    m_Condition.notify_one();
    return true;
  };

  void stop() {
    m_bEnable = false;
    m_Condition.notify_one();
  };

  void kill() {
    m_bRun = false;
    m_bEnable = false;
    m_Condition.notify_one();
  }

  bool join() {
    bool r = false;
    if(m_Thread.joinable()) {
      m_Thread.join();
      r = true;
    }
    return r;
  };

  void _waitUntilEnabled() {
    UniqueLock lg(m_Mutex);
    m_Condition.wait(lg, [this]{ return m_bEnable || !m_bRun; });
  };

  void _timeWork() {
    m_u64FunctionStartTimestamp = TIMESTAMP_MS;
    work();
    m_u64FunctionEndTimestamp = TIMESTAMP_MS;
    m_u64FunctionTime = m_u64FunctionEndTimestamp - m_u64FunctionStartTimestamp;
    if(m_u64FunctionTime > m_u32CycleTime_ms) {
      m_bWorkTooExpensive = true;
      m_u32ModifiedInterval = 0;
    } else {
      m_bWorkTooExpensive = false;
      m_u32ModifiedInterval = m_u32CycleTime_ms - m_u64FunctionTime;
    }
  };

  void run() {
    _waitUntilEnabled();
    onStart();
    while(m_bRun) {
      _waitUntilEnabled();

      while(m_bEnable) {
        _timeWork();
        std::this_thread::sleep_for(Milliseconds(m_u32CycleTime_ms));
      }
    }
    onStop();
  };

  virtual void work() {};
  virtual void onStart() {};
  virtual void onStop() {};

#ifdef ENABLE_DRAW_FUNCTIONS
  virtual void draw() {};
  virtual void drawSettings() {};
#endif

  [[nodiscard]] bool hasError() const {
    return !m_sError.empty();
  };

  [[nodiscard]] std::string getError() const {
    return m_sError;
  };

  [[nodiscard]] bool isRunning() const {
    return m_bRun;
  };

  [[nodiscard]] bool isEnabled() const {
    return m_bEnable;
  }

  [[nodiscard]] uint32_t getCycleTime() const {
    return m_u32CycleTime_ms;
  };

  void setCycleTime(uint32_t i_u32CycleTime) {
    m_u32CycleTime_ms = i_u32CycleTime;
  };

  [[nodiscard]] ModuleInformation getInformation() const {
    return m_Information;
  }

  std::vector<ModuleInformation> getModuleDependencies() {
    return m_Dependencies;
  }

  void setDependency(const std::string& i_sName, IModule* i_pModule) {
    m_DependencyMap[i_sName] = i_pModule;
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
  static T* load(const Path& path, bool verbose = false) {
    T* r = nullptr;
    (void) dlerror(); // clearing any previous errors
    if (std::filesystem::exists(path) && path.has_extension() && path.extension() == ".so") {
      void* h = dlopen(std::filesystem::absolute(path).c_str(), RTLD_LAZY);
      if(h != nullptr) {
        typedef T* create_t();
        auto* c = (create_t*) dlsym(h, "create"); // NOLINT(clion-misra-cpp2008-5-2-4)
        auto e = dlerror();
        if(e == nullptr) {
          r = c();
        }
      }
    }
    return r;
  }

  /*!
   * load all shared objects in a directory
   * @tparam T
   * @param path std::string
   * @param verbose bool
   * @return std::vector<T*>
   */
  template<typename T>
  static std::vector<T*> loadDirectory(const Path& path, bool verbose = false) {
    std::vector<T*> r;
    for (const auto& e: std::filesystem::directory_iterator(path)) {
      const auto& p = e.path();
      auto *ptr = load<T>(p, verbose);
      if(ptr != nullptr) {
        r.push_back(ptr);
      }
    }
    return r;
  }

  /*!
   * load all shared objects in a directory recursively
   * @tparam T
   * @param path
   * @param verbose
   * @return
   */
  template<typename T>
  static std::vector<T*> loadDirectoryRecursive(const Path& path, bool verbose = false) {
    std::vector<T*> r;
    for (const auto& e: std::filesystem::recursive_directory_iterator(path)) {
      const auto& p = e.path();
      auto *ptr = load<T>(p, verbose);
      if(ptr != nullptr) {
        r.push_back(ptr);
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
  static ModuleType* loadModule(const Path& path, bool verbose = false) {
    return load<ModuleType>(path, verbose);
  }
};

class ModuleManager {
  std::vector<IModule*> m_Modules;
#ifdef ENABLE_DRAW_FUNCTIONS
  std::atomic_uint32_t m_u32VisibleModule = 0;
#endif

  void resolveModuleDependencies() {
    for(IModule* module : m_Modules) {
      auto moduleDependencies = module->getModuleDependencies();
#ifdef USE_OHLOG
      if(!moduleDependencies.empty()) {
        DLOGA("Loading %i dependencies for module '%s'", moduleDependencies.size(), module->getInformation().toString().c_str());
      }
#endif
      for(const auto& dependency : moduleDependencies){
        IModule *dep = getModuleByInformation(dependency);
        if(dep != nullptr) {
          module->setDependency(dependency.getName(), dep);
        }
#ifdef USE_OHLOG
        else {
          WLOGA("Failed to find dependency '%s' for module '%s", dependency.toString().c_str(), module->getInformation().toString().c_str());
        }
#endif
      }
    }
  }

public:
  explicit ModuleManager(const std::filesystem::path& i_Path, bool i_bRecursive = false, bool i_bVerbose = false) {
    if(i_bRecursive) {
      m_Modules = ModuleLoader::loadDirectoryRecursive<IModule>(i_Path, i_bVerbose);
    } else {
      m_Modules = ModuleLoader::loadDirectory<IModule>(i_Path, i_bVerbose);
    }
    resolveModuleDependencies();
  }

  ~ModuleManager() {
    for(IModule* module : m_Modules) {
      delete module;
    }
  }

  std::string getModuleNames() {
    std::stringstream r;
    for(IModule* m : m_Modules) {
      r << m->getInformation().getName() << ";";
    }
    return r.str();
  }

  uint32_t getModuleCount() {
    return m_Modules.size();
  }

  IModule* getVisibleModule() {
    return m_Modules[m_u32VisibleModule];
  }

  void setModuleVisible(uint32_t i_u32ModuleIndex) {
    m_u32VisibleModule = i_u32ModuleIndex;
  }

  IModule* getModuleByInformation(const ModuleInformation& i_Information) {
    for(IModule* module : m_Modules) {
      if(module->getInformation().toString() == i_Information.toString()) {
        return module;
      }
    }
    return nullptr;
  }

  IModule* getModuleByName(const std::string& i_sName) {
    for(IModule* module : m_Modules) {
      if(module->getInformation().getName() == i_sName) {
        return module;
      }
    }
    return nullptr;
  }
};

#endif //LIBMODULEPP_MODULEPP_H
