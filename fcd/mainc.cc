/********************************************************************************************
 * Copyright Liquid Motion, Inc 2016 All RIghts reserved
 ********************************************************************************************/


//#define ALLJOYN_VERSION 15.09

#include <iostream>

#include <qcc/Debug.h>
#include <qcc/Log.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>
#include <alljoyn/version.h>

#include "PresenceContributor.h"
#define QCC_MODULE "fcd"

qcc::String busName ="fcd";
  
void doHelp(char * name) {
  std::cout << "Usage: " << name << " [options] " << std::endl << "options: " << std::endl 
	    << "-r - start AllJoyn router" << std::endl << "-h - print this" << std::endl
	    << " -d descritpor - descriptor for the entity" << std::endl
	    << "-m mac - mac of the entity" << std::endl
	    << "-p [0|1] - presence " << std::endl;
}
int 
main(int argc, char* argv[])
{

  qcc::String descriptor;
  qcc::String mac;
  bool isPresent = false;
  bool isRouter = false;
  int opt;
  int i;
  //  QCC_SetDebugLevel(QCC_MODULE, 7);
  while ((opt = getopt(argc, argv, "hrd:m:p:")) != -1) {
    switch (opt) {
    case 'h':
      doHelp(argv[0]);
      exit(EXIT_SUCCESS);
    case 'r':
      isRouter = true;
      break;
    case 'd':
      descriptor = qcc::String(optarg);
      break;
    case 'm':
      mac = qcc::String(optarg);
      break;
    case 'p':
      i = atoi(optarg);
      if(0 == i) {
	isPresent = false;
      } else if (1 == i) {
	isPresent = true;
      } else {
	doHelp(argv[0]);
      }
      break;
      
    default: /* '?' */
      doHelp(argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  printf("AllJoyn Library version: %s\n", ajn::GetVersion());
  printf("AllJoyn Library build info: %s\n", ajn::GetBuildInfo());

  QStatus status = ER_OK;
 
  status = AllJoynInit();
  if (ER_OK != status) {
    return false;
  }
  if(true == isRouter) {
    status = AllJoynRouterInit();
    if (ER_OK != status) {
      AllJoynShutdown();
      return false;
    }
  }
  
  
  
  ajn::BusAttachment * bus;
  bus = new ajn::BusAttachment(busName.c_str(), true);
  if(NULL == bus) {
    AllJoynShutdown();
    return false;
  }


  lm::PresenceContributor * contributer = new lm::PresenceContributor(bus);
  // Create the interfaces

  contributer->Contribute(descriptor, mac, isPresent);
  sleep(1);
  AllJoynShutdown();
}
 
