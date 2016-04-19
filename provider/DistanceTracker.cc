/******************************************************************************
 * Copyright (c) 2015, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#include "LsInclude.h"


using namespace ajn;
using namespace std;
using namespace qcc;


DistanceTracker::DistanceTracker(qcc::String path, ajn::BusAttachment* msgBus, LsDatabase* lsDb, LsManager* lsMan) 
        : LsTracker(1, path), BusObject(path.c_str()), DbListener(path), cacheMutex()
{
    LsTracker::intf = msgBus->GetInterface(org_alljoyn_locationservices::DistanceTracker::InterfaceName);
    LsTracker::msgBus = msgBus;
    LsTracker::busObject = this;
    LsTracker::lsDb = lsDb;
    LsTracker::lsMan = lsMan;

    LsTracker::dbFilter = NULL;
    LsTracker::dbCache = NULL;
}

DistanceTracker::~DistanceTracker()
{
    QCC_DbgPrintf(("~DistanceTracker"));

    /* Deregister listeners. */
    lsDb->DeregisterListener(this);

    /* Reclaim memory. */
    delete trackerDevice;
    delete dbFilter;
    delete dbCache;

    /* Unregister the bus object. */
    msgBus->UnregisterBusObject(*this);
}

QStatus DistanceTracker::Start(ajn::Message& msg)
{
    QStatus status = (intf != NULL) ? ER_OK : ER_FAIL;

    cacheMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Get the fixed tracker device/signature. */
        if (status == ER_OK) {
            trackerDevice = new DbDevice(msg->GetArg(0)->v_struct.members[0].v_string.str,
                                         msg->GetArg(0)->v_struct.members[1].v_string.str);
            if (trackerDevice == NULL) {
                status = ER_FAIL;
            } else {
                trackerSignature = lsDb->DeviceSignature(trackerDevice->uuid, trackerDevice->mac);
            }
        }

        /* Build the tracker filter. */
        if (status == ER_OK) {
            qcc::String uuid = msg->GetArg(1)->v_struct.members[0].v_string.str;
            qcc::String mac = msg->GetArg(1)->v_struct.members[1].v_string.str;
            int listNum = (int)(msg->GetArg(2)->v_array.GetNumElements());
            const MsgArg* listArgs = msg->GetArg(2)->v_array.GetElements();
            
            dbFilter = new DbFilter(uuid, mac);
            for (int i = 0; i < listNum; i++) {
                dbFilter->list.push_back(DbDevice(listArgs[i].v_struct.members[0].v_string.str,
                                                  listArgs[i].v_struct.members[1].v_string.str));
            }
        }

        /* Get the distance delta value to use. */
        trackerDelta = (int64_t)msg->GetArg(3)->v_double;

        /* Setup the listeners. */
        if (status == ER_OK) {
            status = lsDb->RegisterListener(this);
        }

        /* Setup the cache. */
        if (status == ER_OK) {
            dbCache = new DbResult();
            status = lsDb->FilterDevices(trackerDevice, dbFilter, dbCache);
        }

        /* Setup the method handlers. */
        if (status == ER_OK) {
            const MethodEntry handlers[] = {
                { intf->GetMember("KeepAlive"), static_cast<MessageReceiver::MethodHandler>(&DistanceTracker::KeepAlive) },
                { intf->GetMember("Query"), static_cast<MessageReceiver::MethodHandler>(&DistanceTracker::Query) },
            };
            status = AddInterface(*intf, UNANNOUNCED);
            if (status == ER_OK) {
                status = AddMethodHandlers(handlers, ArraySize(handlers));
            }
        }
        
        /* Register the bus object. */
        if (status == ER_OK) {
            status = msgBus->RegisterBusObject(*this);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("Start Exception: %s\n", e.what()));
        status = ER_FAIL;
    }
    cacheMutex.Unlock();
    
    QCC_DbgPrintf(("Start: %s", QCC_StatusText(status)));
 
    return status;
}

void DistanceTracker::DetectEvent(qcc::String sensorSignature, 
                                  qcc::String entitySignature,
                                  uint32_t timestamp, 
                                  bool detect) 
{
    //QCC_DbgPrintf(("DetectEvent"));
    return;
}

void DistanceTracker::RangeEvent(qcc::String sensorSignature, 
                                 qcc::String entitySignature,
                                 uint32_t timestamp, 
                                 double range) 
{
    //QCC_DbgPrintf(("RangeEvent"));

    if ((entitySignature == trackerSignature) &&
        (CacheChanged(sensorSignature, timestamp, range))) {
        TrackingSignal(sensorSignature, range);
    }

    if ((sensorSignature == trackerSignature) &&
        (CacheChanged(entitySignature, timestamp, range))) {
        TrackingSignal(entitySignature, range);
    }
    return;
}

QStatus DistanceTracker::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QStatus status;

    try {
        if (!strcmp(ifcName, org_alljoyn_locationservices::DistanceTracker::InterfaceName)) {
            if (strcmp(propName, "Version") == 0) {
                status = val.Set("q", trackerVersion);

            } else if (strcmp(propName, "Tracker") == 0) {
                status = val.Set("o", trackerPath.c_str() );

            } else {
                status = ER_BUS_NO_SUCH_PROPERTY;
            }
        } else {
            status = ER_BUS_NO_SUCH_PROPERTY;
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("Get Exception: %s\n", e.what()));
        status = ER_FAIL;
    }
    return status;
}

void DistanceTracker::KeepAlive(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("KeepAlive"));

    msgBus->EnableConcurrentCallbacks();

    LifetimeExtension();
}

void DistanceTracker::Query(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("Query"));

    msgBus->EnableConcurrentCallbacks();

    cacheMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Refresh the cache */
        delete dbCache;
        dbCache = new DbResult();
        lsDb->FilterDevices(trackerDevice, dbFilter, dbCache);

        /* Setup the query reply and send it. */
        qcc::String *uuidValues = new qcc::String[dbCache->records.size()];
        qcc::String *macValues = new qcc::String[dbCache->records.size()];
        MsgArg* msgValues = new MsgArg[dbCache->records.size()];
        msgValues->SetOwnershipFlags(MsgArg::OwnsArgs, true);

        std::vector<DbRecord*>::iterator it;
        size_t n = 0;   

        qcc::String deviceUuid;
        qcc::String deviceMac;
     
        for(it = dbCache->records.begin(); it != dbCache->records.end(); it++, n++) {
            uuidValues[n] = lsDb->DeviceUuid((*it)->discoverySignature);
            macValues[n] = lsDb->DeviceMac((*it)->discoverySignature);
            double range = ((*it)->rangeTimestamp != 0) ? (*it)->rangeDiscovery : -1; 
            //double range = (*it)->rangeDiscovery;

            QCC_DbgPrintf(("RANGE = %f", range));

            msgValues[n].Set("(ssd)", uuidValues[n].c_str(), macValues[n].c_str(), range);
        }

        if (n > 0) {
            MsgArg msgArg("a(ssd)", dbCache->records.size(), msgValues);
            MethodReply(msg, &msgArg, 1);
        } else {
            QStatus err = (QStatus)ER_LS_NO_MATCHES;
            QCC_DbgPrintf(("Query - no elements found"));
            MethodReply(msg,err);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("Query exception: %s\n", e.what()));
    }
    cacheMutex.Unlock();

    //QCC_DbgPrintf(("Query Complete"));
}

bool DistanceTracker::CacheChanged(qcc::String signature, uint32_t timestamp, double range) {
    //QCC_DbgPrintf(("CacheChanged = %f : %f", range, trackerDelta));

    bool result = false;

    cacheMutex.Lock(MUTEX_CONTEXT);
    try {
        if (dbCache != NULL) {
            /* Determine if an existing property item needs to be updated. */
            bool found = false;
            std::vector<DbRecord *>::iterator it;
            for (it = dbCache->records.begin(); !found && (it != dbCache->records.end()); it++) {
                if ((found = (signature == (*it)->discoverySignature))) {
                    if ((result = (abs(range - (*it)->rangeDiscovery) >= trackerDelta))) {
                        (*it)->rangeTimestamp = timestamp;
                        (*it)->rangeDiscovery = range;
                        result = true;
                    }
                }
            } 
            
            /* Determine if a new property item was included. */
            if (!found && (lsDb->FilterSignature(signature, dbFilter))) {
                DbRecord* resultRecord = new DbRecord(signature);
                resultRecord->rangeTimestamp = timestamp;
                resultRecord->rangeDiscovery = range;
                dbCache->records.push_back(resultRecord);
                result = true;
            }
        }
        
    } catch(std::exception& e) {
        QCC_DbgPrintf(("CacheChanged Exception: %s\n", e.what()));
        result = false;
    }
    cacheMutex.Unlock();

    return result;
}

void DistanceTracker::TrackingSignal(qcc::String signature, double range) {
    QCC_DbgPrintf(("TrackingSignal"));

    try {
        qcc::String deviceUuid = lsDb->DeviceUuid(signature);
        qcc::String deviceMac = lsDb->DeviceMac(signature);
        
        MsgArg* msgValues = new MsgArg;
        msgValues->SetOwnershipFlags(MsgArg::OwnsArgs, true);      
        msgValues->Set("(ssd)", deviceUuid.c_str(), deviceMac.c_str(), range);
        
        busObject->Signal(NULL, 
                          SESSION_ID_ALL_HOSTED, 
                          *(intf->GetMember("Tracking")), 
                          msgValues, 
                          1);

    } catch(std::exception& e) {
        QCC_DbgPrintf(("TrackingSignal Exception: %s\n", e.what()));
    }
}



