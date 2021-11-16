//
// Created by Pascal Eberlein on 23.03.21.
//
/*
 * MIT License
 *
 * Copyright ohlog 2021 Pascal Eberlein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LOGGER_OHLOG_H
#define LOGGER_OHLOG_H

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

template <typename T> using SubscribeCallback = std::function<void(const T &)>;
template <typename T>
using CallbackMap = std::map<std::string, SubscribeCallback<T>>;
template <typename T>
using SubscriptionMap = std::map<std::string, CallbackMap<T>>;

/*!
 * publish subscribe library for inter class communication
 */
template <typename T> class TPubSub {
  inline static TPubSub *instance = nullptr;

  std::mutex mtx;
  SubscriptionMap<T> subscriptions;

public:
  TPubSub() = default;

  /*!
   * returns a PubSub instance, use this to instantiate the PubSub object
   * @return Pubsub*
   */
  static TPubSub *getInstance() {
    if (instance == nullptr)
      instance = new TPubSub<T>();
    return instance;
  }

  /*!
   * @return getInstance();
   */
  static TPubSub *get() { return getInstance(); }

  /*!
   * delete the current instance
   */
  static void destroy() { delete instance; }

  /*!
   * generates a random string
   * @param length
   * @param charset
   * @return std::string with specified length
   */
  static std::string
  generateString(int length = 64,
                 std::string charset =
                     "123456789BCDFGHJKLMNPQRSTVWXZbcdfghjklmnpqrstvwxz") {
    std::string r;
    for (int _ = 0; _ < length; _++)
      r += charset[rand() % (charset.size() - 1)];
    return r;
  };

  /*!
   * publish a message to this channel
   * @param channel
   * @param msg
   */
  void publish(const std::string &channel, T msg) {
    mtx.lock();
    for (const auto &topic : subscriptions) {
      for (const auto &kv : topic.second)
        kv.second(msg);
    }
    mtx.unlock();
  };

  /*!
   * subscribe to a topic
   * @param channel
   * @param callback
   * @return callback name, should be stored if you wanna unsubscribe
   */
  std::string subscribe(const std::string &channel,
                        SubscribeCallback<T> callback) {
    auto r = generateString();
    mtx.lock();
    subscriptions[channel][r] = callback;
    mtx.unlock();
    return r;
  }

  /*!
   * subscribe to a topic
   * @param channel
   * @param callbackName
   * @param callback
   */
  void subscribe(const std::string &channel, const std::string &callbackName,
                 SubscribeCallback<T> callback) {
    mtx.lock();
    subscriptions[channel][callbackName] = callback;
    mtx.unlock();
  }

  /*!
   * unsubscribe from a topic
   * @param channel
   * @param callbackName
   */
  void unsubscribe(const std::string &channel,
                   const std::string &callbackName) {
    mtx.lock();
    subscriptions[channel].erase(callbackName);
    mtx.unlock();
  }

  /*!
   * remove all callbacks for a topic
   * @param channel
   */
  void clear(const std::string &channel) {
    mtx.lock();
    subscriptions[channel].clear();
    mtx.unlock();
  }

  /*!
   * remove all callbacks
   */
  void clear() {
    mtx.lock();
    subscriptions.clear();
    mtx.unlock();
  }
};

class StrPubSub : public TPubSub<const std::string &> {
public:
  StrPubSub() = default;
};

#define DEFAULT_FORMAT "%Y.%m.%d %H:%M:%S"
#define GET_FILENAME std::filesystem::path(__FILE__).filename()

namespace ohlog {
enum LogLevel { DEBUG = 0, INFO, WARNING, ERROR };

class Logger {
public:
  explicit Logger(std::string i_sLogFilePath)
      : m_sLogFilePath(std::move(i_sLogFilePath)) {
    if (isLoggingToFile()) {
      m_LogFileStream = std::ofstream(m_sLogFilePath);
    }
    queue.subscribe("logMsg", [this](auto &&PH1) {
      writeToLog(std::forward<decltype(PH1)>(PH1));
    });
  }

  /*!
   * get a Logger instance
   * @return pointer to initialized Logger
   */
  [[maybe_unused]] static Logger *get(const std::string &filePath = "") {
    if (self == nullptr)
      self = new Logger(filePath);
    return self;
  }

  /*!
   * get the current timestamp
   * @param format std::string, the format to use
   * @return the current timestamp formatted as a string
   */
  static std::string
  getCurrentTimestamp(const std::string &format = DEFAULT_FORMAT) {
    std::stringstream o;
    const auto t = std::time(nullptr);
    const auto tm = *std::localtime(&t);
    o << std::put_time(&tm, format.c_str());
    return o.str();
  }

  /*!
   * log to cli
   * @tparam Args
   * @param tag std::string, the prefix for the line
   * @param msg std::string, the message to display
   * @param level LogLevel, the loglevel
   * @param arguments
   */
  template <typename... Args>
  void log(const std::string &tag, const std::string &msg,
           LogLevel level = INFO, Args... arguments) {
    std::stringstream o;
    switch (level) {
    case DEBUG:
      o << "D ";
      break;
    case INFO:
      o << "I ";
      break;
    case WARNING:
      o << "W ";
      break;
    case ERROR:
      o << "E ";
      break;
    }
    o << getCurrentTimestamp() << " " << tag << ": " << msg;
    std::string formatStr = o.str();
    int lineLength = snprintf(nullptr, 0, formatStr.c_str(), arguments...) + 1;
    char line[lineLength + 1];
    snprintf(line, lineLength, formatStr.c_str(), arguments...);
    std::string logLine(line);
    std::cout << logLine << std::endl;
    if (isLoggingToFile()) {
      queue.publish(m_sLogChannel, logLine);
    }
  }

  void writeToLog(const std::string &logLine) {
    m_LogFileStream << logLine << std::endl;
    m_LogFileStream.flush();
  }

  /*!
   * shorthand function for log
   * @tparam Args
   * @param tag
   * @param msg
   * @param arguments
   */
  template <typename... Args>
  [[maybe_unused]] void d(const std::string &tag, const std::string &msg,
                          Args... arguments) {
    log(tag, msg, DEBUG, arguments...);
  }

  /*!
   * shorthand function for log
   * @tparam Args
   * @param tag
   * @param msg
   * @param arguments
   */
  template <typename... Args>
  [[maybe_unused]] void i(const std::string &tag, const std::string &msg,
                          Args... arguments) {
    log(tag, msg, INFO, arguments...);
  }

  /*!
   * shorthand function for log
   * @tparam Args
   * @param tag
   * @param msg
   * @param arguments
   */
  template <typename... Args>
  [[maybe_unused]] void w(const std::string &tag, const std::string &msg,
                          Args... arguments) {
    log(tag, msg, WARNING, arguments...);
  }

  /*!
   * shorthand function for log
   * @tparam Args
   * @param tag
   * @param msg
   * @param arguments
   */
  template <typename... Args>
  [[maybe_unused]] void e(const std::string &tag, const std::string &msg,
                          Args... arguments) {
    log(tag, msg, ERROR, arguments...);
  }

  bool isLoggingToFile() { return !m_sLogFilePath.empty(); }

private:
  inline static std::string m_sLogChannel = "logMsg";
  inline static Logger *self = nullptr;

  std::string m_sLogFilePath;
  std::ofstream m_LogFileStream;
  StrPubSub queue;
};
} // namespace ohlog

#define OHLOG ohlog::Logger::get()
#define DLOG(msg) OHLOG->d(GET_FILENAME, msg)
#define DLOGA(msg, args...) OHLOG->d(GET_FILENAME, msg, args)
#define ILOG(msg) OHLOG->i(GET_FILENAME, msg)
#define ILOGA(msg, args...) OHLOG->i(GET_FILENAME, msg, args)
#define WLOG(msg) OHLOG->w(GET_FILENAME, msg)
#define WLOGA(msg, args...) OHLOG->w(GET_FILENAME, msg, args)
#define ELOG(msg) OHLOG->e(GET_FILENAME, msg)
#define ELOGA(msg, args...) OHLOG->e(GET_FILENAME, msg, args)

#endif // LOGGER_OHLOG_H
