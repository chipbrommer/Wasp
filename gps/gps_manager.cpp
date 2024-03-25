#include "gps_manager.h"

GpsManager::GpsManager(LogClient& logger, GpsOptions gps) : m_logger(logger), m_gps(gps) {}

GpsManager::~GpsManager() {}