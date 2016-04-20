/********************************************************************************************
 * Copyright Liquid Motion, Inc 2016 All RIghts reserved
 ********************************************************************************************/


#include <qcc/Debug.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/Session.h>
#include <ls/PresenceContributorInterface.h>

#include "PresenceContributor.h"


#define ArraySize(a)  (sizeof(a) / sizeof(a[0]))

#define QCC_MODULE "fcd"


namespace lm {

  PresenceContributor::PresenceContributor(ajn::BusAttachment * bus) : mBus(bus), 
								       cdDescriptor("Sensor"),
								       cdMac("00:00:00:00:00:00")
  {

    QStatus status;
    mSid = 0;
#if 1
    status = mBus->CreateInterfacesFromXml(org_alljoyn_locationservices::presence::Contributor::INTERFACE_XML);
    if(ER_OK != status) {
      QCC_DbgPrintf(("CreateInterfaces failed %s", QCC_StatusText(status)));
      AllJoynShutdown();
    } else {
      QCC_DbgPrintf(("Create Interface succeeded"));
    }
#endif    
    status = mBus->Start();
    if(ER_OK != status) {
      QCC_DbgPrintf(("Start failed %s", QCC_StatusText(status)));
      AllJoynShutdown();
    }
    
    status = mBus->Connect();
    if(ER_OK != status) {
      QCC_DbgPrintf(("Connect failed %s", QCC_StatusText(status)));
      AllJoynShutdown();
      
    }
    if(ER_OK == status) {
      mObserver = new ajn::Observer(*mBus,(const char **) &org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME,1);
      mObserver->RegisterListener(*this);
      mBus->RegisterAboutListener(*this);
      status = mBus->WhoImplements(org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME);
    }
    QCC_DbgPrintf(("presence Contributor constructor status %s", QCC_StatusText(status)));

  }

  QStatus
  PresenceContributor::Contribute(qcc::String & descriptor, qcc::String & mac, bool isPresent)
  {
    QStatus status = ER_OK;
    ajn::MsgArg cdMsg("(ss)", cdDescriptor.c_str(), cdMac.c_str()); 
    ajn::MsgArg entityMsg("(ss)", descriptor.c_str(), mac.c_str());
    ajn::MsgArg presentMsg("b", isPresent);
    ajn::MsgArg msg[] = { cdMsg, entityMsg, presentMsg };
    while(mProxies.empty()) {
      sleep(1);
    }
    for (std::vector<ajn::ProxyBusObject*>::iterator it = mProxies.begin();
	 (status == ER_OK) && (it != mProxies.end());
	 it++) {
      QCC_DbgPrintf(("calling EntityPresence"));
      status = (*it)->MethodCall(org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME, 
				 "UpdatePresenceInfo", 
				 msg, 
				 ArraySize(msg));
      if(ER_OK != status) {
	QCC_DbgPrintf(("Contribute %s",QCC_StatusText(status)));
      }  else  {
	QCC_DbgPrintf(("Contributed"));
      }
    }      
      
    return status;

  }

  void 
  PresenceContributor::ObjectDiscovered(ajn::ProxyBusObject& proxy) 
  {
    
    QCC_DbgPrintf(("PresenceContributor Object Discovered Name: %s UniqueName: %s Path: %s", proxy.GetServiceName().c_str(), proxy.GetUniqueName().c_str(), proxy.GetPath().c_str()));
#if 0    
    ajn::ProxyBusObject * pbo = new ajn::ProxyBusObject(*mBus,  proxy.GetServiceName().c_str(), 
					      proxy.GetUniqueName().c_str(), 
					      proxy.GetPath().c_str(),mSid);
    mProxies.push_back(pbo);
#endif
  }

  void 
  PresenceContributor::ObjectLost(ajn::ProxyBusObject& proxy)
  {
    QCC_DbgPrintf(("PresenceContributor Object Lost ", proxy.GetServiceName().c_str()));

  }


  void 
  PresenceContributor::Announced(const char* name, uint16_t version, 
					      ajn::SessionPort port,
					      const ajn::MsgArg& objectDescriptionArg, 
					      const ajn::MsgArg& aboutDataArg)
  {
    QStatus status = ER_OK;
    QCC_DbgPrintf(("PresenceContributor found %s", name));
    ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
    ajn::AboutObjectDescription objs(objectDescriptionArg);
    ajn::AboutData aboutData(aboutDataArg);

    mBus->EnableConcurrentCallbacks();

    size_t numPaths = objs.GetInterfacePaths(org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME, NULL, 0);
    QCC_DbgPrintf(("Announced found %d paths",numPaths));

    if(numPaths) {
      const char **paths;
      paths = new const char *[numPaths];
      objs.GetInterfacePaths(org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME, paths, numPaths);
      for (size_t i = 0; i < numPaths; ++i) {
	QCC_DbgPrintf(("Announced: %s (%s)", name, paths[i]));
	ajn::ProxyBusObject* proxyObj = new ajn::ProxyBusObject(*mBus, name, paths[i], mSid);
	status = proxyObj->AddInterface(org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME);
	if (status == ER_OK) {
	  mProxies.push_back(proxyObj);
	}
      }
      delete [] paths;
      status = mBus->JoinSession(name, port, NULL, mSid, opts);
      if (status != ER_OK ) {
	QCC_DbgPrintf(( "PresenceContributor  JoinSession failed %s for %s", QCC_StatusText(status),  name));
	return;
      } 
      QCC_DbgPrintf(("Joined session %d",mSid));
    }
  }

}
