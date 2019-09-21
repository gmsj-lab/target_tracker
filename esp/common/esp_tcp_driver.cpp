/*
 * EspTcpDriver.cpp
 *
 *  Created on: 7 oct. 2016
 *      Author: gmsj
 */

#include "esp_tcp_driver.h"

uint8_t EspTcpDriver::nextClientNumber = 0 ;
TcpConnectionClient  *EspTcpDriver::clientTable [ NB_MAX_CLIENTS ] ;


