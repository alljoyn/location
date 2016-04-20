/********************************************************************************************
 * Copyright Liquid Motion, Inc 2016 All RIghts reserved
 ********************************************************************************************/

#ifndef _PRESENCE_CONTRIBUTER_H_
#define _PRESNECE_CONTRIBUTER_H_

#include <vector>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/Observer.h>
#include <alljoyn/BusAttachment.h>


namespace lm {
  
  class PresenceContributor :
    public ajn::AboutListener,
    public ajn::MessageReceiver,
    public ajn::Observer::Listener

  {

  public:
    PresenceContributor(ajn::BusAttachment * bus);

    QStatus Contribute(qcc::String & descriptor, qcc::String & mac, bool isPresent);

    virtual void ObjectDiscovered(ajn::ProxyBusObject& proxy);
    
    virtual void ObjectLost(ajn::ProxyBusObject& proxy);
    // AboutListener
    virtual void Announced(const char* name, uint16_t version, ajn::SessionPort port,
			   const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);
    
  private:
    ajn::BusAttachment * mBus;
    //bool isReady;
    qcc::String cdDescriptor;
    qcc::String cdMac;
    ajn::SessionId mSid;
    //    ajn::ProxyBusObject * mProxyBusObject;
    ajn::Observer * mObserver;
    std::vector<ajn::ProxyBusObject*> mProxies;
    //bool mIsReady;
  };
}
  
#endif // _PRESENCE_CONTRIBUTER_H_
