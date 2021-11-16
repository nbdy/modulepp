//
// Created by nbdy on 16.11.21.
//

#ifndef MODULEPP_MODULES_GPS_GPS_H_
#define MODULEPP_MODULES_GPS_GPS_H_

#include "modulepp.h"

#include "gps.h"

class GPS : public IModule {
  std::mutex m_GpsDataMutex;
  struct gps_data_t m_GpsData {};
  std::string m_sHost = "localhost";
  std::string m_sPort = "2947";
  bool m_bReadError = false;

#ifdef ENABLE_SHARED_DATA
  void onNewGpsData(nlohmann::json& i_Data);
#endif

public:
  GPS();

  void onStart() override;
  void onStop() override;

  void work() override;

  void setHost(const std::string& i_sHost);
  void setPort(const std::string& i_sPort);

  void getGpsData(struct gps_data_t& o_GpsData);
};

#endif // MODULEPP_MODULES_GPS_GPS_H_
