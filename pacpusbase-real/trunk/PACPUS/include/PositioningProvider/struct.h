/*********************************************************************
//  created:    2006/10/16 - 11:24
//  filename:   struct.h
//
//  author:     Pierre-Michel Bonnifait Copyright Heudiasyc UMR UTC/CNRS 6599
//
//  version:    $Id: struct.h 277 2007-09-04 15:47:11Z gdherbom $
//
//  purpose:    Definition of all the program's structures.
// 
//	todo :		Changer le nom du fichier struct.h
*********************************************************************/




#ifndef _STRUCT_H_
# define _STRUCT_H_

// Standard includes which are necesary ... 

# include <windows.h>
# include <stdio.h>
# include <iostream.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include <fstream>


// Qt includes ...

# include <qthread.h>
# include <qptrlist.h> 
# include <qapplication.h>
# include <qsemaphore.h>
# include <qwt_plot.h>
# include <qwt_plot_canvas.h>
# include <qwt_scale.h>
# include <qwt_scldraw.h>
# include <qwt_math.h>
# include <qlineedit.h>
# include <qcheckbox.h>
# include <qwt_compass.h>
# include <qwt_dial_needle.h>
# include <qptrlist.h>



// specific PACPUS includes ...

# include "pacpusTools/matrice.h"
# include "pacpusTools/shmem.h"
# include "kernel/road_time.h"
# include "kernel/pacpus.h"
# include "kernel/componentManager.h"
# include "kernel/ComponentFactory.h"
# include "kernel/ComponentFactoryBase.h"
# include "kernel/componentBase.h"
# include "mapdata.h"


//# include "../roads_extraction_dll.hpp"
//# include "../moc/positioning_provider_frame.h"
//# include "../../include/BeNomad/BTypes.h"
//# include "../rue_data.h"



// Constantes definition ... 

#define INTEGRITY_THRESHOLD     3



// structure that contains the sensors data: GPS, ABS and gyro
struct s_SensorsData
{
	 double				yGps;
   double				xGps;
   float aGps,bGps,phiGps,sigma_latGps,sigma_lonGps; //////////add gst
   float				absArG;
   float				absArD;
   float				speed;
   float				yaw;
   long				ind_qualite;
   road_time_t		timeOfGpsData;
   road_time_t		timeOfAbsData;
   road_time_t		timeOfGyroData;
};



// Recording structure for the roads sent by BeNomad -> MapMatching.cpp 
struct s_MapCache
{
  TB_2D_LONGPOINT	PtCentral;
  TBuint32			Rayon ;
  hds_polyline*		Routes; 
  unsigned long		NbRoutes;
  bool             ready; 
};



// convenient structure that defines a position X,Y
struct s_position
{
	 double				x;
   double				y;
};



// the definition of a segment (a part of a road) 
typedef struct
{
  TB_2D_LONGPOINT	ext1;//premi�re extr�mit� du segment
	 TB_2D_LONGPOINT	ext2;//deuxi�me extr�mit� du segment
   unsigned long		indice; //numero du segment dans la liste de la route 
   double				theta;
}SEGMENT;



// structure for mapmatching using heading information
struct s_CalcBestTheta
{
	 hds_polyline		road;
   SEGMENT			seg;
   int				offset;
   double				theta;
};



// enumeration to detect the data incoming 
enum e_event_synchro
{
  E_GPS, E_ABS, E_GYRO
};



// the different values of the state machine of the PP 
// WAITGPS:       no GPS position received from the last initialization 
// INIT_HEADING:  we know where we are (GPS position), we are waiting a movement 
//                to init the heading 
// TRACKING:      the normal working mode, output available
enum e_Server_mode
{
  WAITGPS, INIT_HEADING, TRACKING
};



// the different values that are understood by the RequestServer 
// E_END:               end of application 
// E_CLIENT_REQUEST:    external request of mapmatching by an external client (VMM for example)
// E_INTERNAL_REQUEST:  internal request of mapmatching, typically done after the computation of the position 
enum e_Request_signal
{
  E_END,
    E_CLIENT_REQUEST,
    E_INTERNAL_REQUEST
};



// MACRO
#define isvalid(x,s) do {																\
  if ((x == NULL) || (x == INVALID_HANDLE_VALUE))							\
{																		\
  fprintf(stderr, "error while opening %s: %d", s, GetLastError());	\
  ::exit (0);															\
}																		\
} while(0);



/* Programm headers' inclusion */
//#include "../../include/tube.hpp"
#include "MapMatching.h"
#include "PositioningComputing.h"
#include "PositioningSensors.h"
#include "PositionProvider.h"
#include "RequestServer.h"
#include "positioningframe.h"
#include "LogFile.h"
#include "MapClientSocket.h"
/* End of headers' include */

#endif /* !_STRUCT_H_ */