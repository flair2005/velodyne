/*********************************************************************
//  created:    2007/01/19 - 14:22
//  filename:   MapFrontEnd.h
//
//  author:     Gerald Dherbomez
// 
//  version:    $Id: MapFrontEnd.h 207 2007-03-31 10:24:42Z gdherbom $
//
//  purpose:    Manage the interface of the application with other 
//              programs 
*********************************************************************/


#ifndef _MAPFRONTEND_H_
#define _MAPFRONTEND_H_


#include "MapServerSocket.h"
#include "MapRoadExtraction.h"

/*
class ShMemRoadExtraction : public ShMem
{
public:
  ShMemRoadExtraction(char * name, int size) : ShMem(name, size){}; 
  ~ShMemRoadExtraction(){}; 
  // Warning : this reimplemented function allocates memory to copy the dynamic 
  // structure of hds_polyline* in shared memory
  // So don't forget to free memory with the freeRoads() method !! 
  void write(void *data, int size); 

  void freeRoads(); 


};
*/

/*
Ajouter la possibilit� d'acc�der aux diff�rents sockets 
Voir a g�rer une liste dans MapServerSocket. 
Pour l'instant une seule application est g�r�e
*/
class MapFrontEnd
{
public:
	MapFrontEnd();
	~MapFrontEnd();

  void send(int identifier, QString data); 
  inline MapRoadExtraction *getMapRoadExtraction() { return roadExtractor; }

protected:
	
private:
  MapServerSocket *socketServer_;  
  MapRoadExtraction *roadExtractor; 


};



#endif