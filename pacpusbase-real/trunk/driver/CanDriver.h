/*********************************************************************
//  created:    2006/08/08 - 10:36
//  filename:   CanDriver.h
//
//  author:     Gerald Dherbomez
// 
//  version:    $Id: CanDriver.h 1312 2013-07-09 15:22:07Z phudelai $
//
//  purpose:    This is a generic class which may be used to control 
//              a CAN interface
// 
//              Only one driver can be used in the same application !
// 
*********************************************************************/



#ifndef _CANDRIVER_H_
#define _CANDRIVER_H_


#define VECTOR_CAN_DRIVER		1
#define NI_CAN_DRIVER			2
#define VECTOR_CANXL_DRIVER     3
#define ESD_CAN_DRIVER			4


#if CAN_DRIVER_TYPE == VECTOR_CAN_DRIVER
#include "VectorCanDriver.h"
#define CanDriverType VectorCanDriver
#elif CAN_DRIVER_TYPE == NI_CAN_DRIVER
#include "NiCanDriver.h"
#define CanDriverType NiCanDriver
#elif CAN_DRIVER_TYPE == PEAK_CAN_DRIVER
#include "PeakCanDriver.h"
#define CanDriverType PeakCanDriver
#elif CAN_DRIVER_TYPE == VECTOR_CANXL_DRIVER
#include "XLVectorCanDriver.h"
#define CanDriverType XLVectorCanDriver
#elif CAN_DRIVER_TYPE == ESD_CAN_DRIVER
#include "EsdCanDriver.h"
#define CanDriverType EsdCanDriver
#else
#define CanDriverType CanDriverUnknown
#endif 



class CanDriver : public CanDriverType
{
public:
  CanDriver(){};
  CanDriver(int channel) : CanDriverType(channel){}; 
  CanDriver(const int channel, const unsigned int speed)
    : CanDriverType(channel, speed){}; 
  CanDriver(char* port, char* mode):CanDriverType(port, mode){};
  ~CanDriver(){};
protected:
  
private:
};



#endif