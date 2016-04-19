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

static qcc::String lowercase( qcc::String myString ) {
    for( size_t pos = 0; pos < myString.length(); pos++ ) {
        myString[pos] = std::tolower(myString[pos]);
    }
    return(myString);
}       

Locationservices::Locationservices(ajn::BusAttachment* msgBus, LsDatabase* lsDb, LsManager* lsMan) 
        : BusObject("/Locationservices"), msgBus(msgBus), lsDb(lsDb), lsMan(lsMan),
          injectMutex(), trackerMutex(), presenceTrackers(), distanceTrackers()
{
    this->SetDescription("en", "LS Object");
    return;
}

Locationservices::~Locationservices()
{
    QCC_DbgPrintf(("~Locationservices"));


    /* Delete the trackers. */
    trackerMutex.Lock(MUTEX_CONTEXT);
    for (vector<PresenceTracker*>::iterator it = presenceTrackers.begin(); it != presenceTrackers.end(); ++it) {
        delete *it;
    }
    for (vector<DistanceTracker*>::iterator it = distanceTrackers.begin(); it != distanceTrackers.end(); ++it) {
        delete *it;
    }
    trackerMutex.Unlock();

    /* Unregister the bus object. */
    msgBus->UnregisterBusObject(*this);
}

QStatus Locationservices::Start()
{
    intf = msgBus->GetInterface(org_alljoyn_locationservices::Locationservices::InterfaceName);
    QStatus status = (intf != NULL) ? ER_OK : ER_FAIL;

    /* Setup the method handlers. */
    if (status == ER_OK) {
        const MethodEntry handlers[] = {
            { intf->GetMember("EntityPresence"), static_cast<MessageReceiver::MethodHandler>(&Locationservices::EntityPresence) },
            { intf->GetMember("EntityRange"), static_cast<MessageReceiver::MethodHandler>(&Locationservices::EntityRange) },
            { intf->GetMember("PresenceSubscribe"), static_cast<MessageReceiver::MethodHandler>(&Locationservices::PresenceSubscribe) },
            { intf->GetMember("DistanceSubscribe"), static_cast<MessageReceiver::MethodHandler>(&Locationservices::DistanceSubscribe) },
        };
        status = AddInterface(*intf, ANNOUNCED);
        if (status == ER_OK) {
            status = AddMethodHandlers(handlers, ArraySize(handlers));
        }
        //if (status == ER_OK) {
        //    QCC_DbgPrintf(("LOCATION SERVICES INTF: %s:%s %s:%s", 
        //                   intf->GetDescriptionLanguage(),
        //                   intf->HasDescription() ? "description exists" : "no description",
        //                   intf->GetMember("HiggnsTest")->description.c_str(),
        //                   intf->GetMember("HiggnsTest")->isSessioncastSignal ? "sessioncast": "???"));
        //}
    }

    /* Register the bus object. */
    if (status == ER_OK) {
        status = msgBus->RegisterBusObject(*this);
    }
    
    QCC_DbgPrintf(("Start: %s", QCC_StatusText(status)));
 
    return status;
}

void Locationservices::Check() {
    //QCC_DbgPrintf(("Check"));

    LsTracker *tracker;

    trackerMutex.Lock(MUTEX_CONTEXT);
    for (vector<PresenceTracker*>::iterator it = presenceTrackers.begin(); it != presenceTrackers.end(); ) {
        tracker = dynamic_cast<LsTracker *>(*it);
        if ((tracker != NULL) && (!tracker->LifetimeRemaining())) {
            delete *it;
            *it = NULL;
            it = presenceTrackers.erase(it);
        } else {
            ++it;
        }
    }
    trackerMutex.Unlock();

    trackerMutex.Lock(MUTEX_CONTEXT);
    for (vector<DistanceTracker*>::iterator it = distanceTrackers.begin(); it != distanceTrackers.end();) {
        tracker = dynamic_cast<LsTracker *>(*it);
        if ((tracker != NULL) && (!tracker->LifetimeRemaining())) { 
            delete *it;
            *it = NULL;
            it = distanceTrackers.erase(it);
        } else {
            it++;
        }
    }
    trackerMutex.Unlock();
}

void Locationservices::EntityPresence(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    //QCC_DbgPrintf(("EntityPresence"));

    injectMutex.Lock(MUTEX_CONTEXT);
    try {
        msgBus->EnableConcurrentCallbacks();

        /* Get the message arguments. */
        qcc::String sensorUuid = lowercase(msg->GetArg(0)->v_struct.members[0].v_string.str);
        qcc::String sensorMac = lowercase(msg->GetArg(0)->v_struct.members[1].v_string.str);
        qcc::String entityUuid = lowercase(msg->GetArg(1)->v_struct.members[0].v_string.str);
        qcc::String entityMac = lowercase(msg->GetArg(1)->v_struct.members[1].v_string.str);
        bool isPresent = msg->GetArg(2)->v_bool;
        //int64_t msgTimestamp = msg->GetTimeStamp();
        uint32_t msgTimestamp = ajn::BusAttachment::GetTimestamp();


        /* Post the information to the database. */
        lsDb->PostDetectDiscovery(isPresent,
                                  msgTimestamp,
                                  DbDevice(sensorUuid, sensorMac),
                                  DbDevice(entityUuid, entityMac));
        MethodReply(msg);

        //QCC_DbgPrintf(("EntityPresence Sensor: %s:%s\n", sensorUuid.c_str(), sensorMac.c_str()));    
        //QCC_DbgPrintf(("EntityPresence Entity: %s:%s\n", entityUuid.c_str(), entityMac.c_str()));
        //QCC_DbgPrintf(("EntityPresence isPresent: %d\n", isPresent));
        //QCC_DbgPrintf(("EntityPresence msgTimestamp: %ld\n", msgTimestamp));        
        

    } catch(std::exception& e) {
        QCC_DbgPrintf(("EntityPresence Exception: %s\n", e.what())); 
    }
    injectMutex.Unlock();
    return;
}

void Locationservices::EntityRange(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    //QCC_DbgPrintf(("EntityRange - BEGIN"));

    injectMutex.Lock(MUTEX_CONTEXT);
    try {
        msgBus->EnableConcurrentCallbacks();

        /* Get the message arguments. */
        qcc::String sensorUuid = lowercase(msg->GetArg(0)->v_struct.members[0].v_string.str);
        qcc::String sensorMac = lowercase(msg->GetArg(0)->v_struct.members[1].v_string.str);
        qcc::String entityUuid = lowercase(msg->GetArg(1)->v_struct.members[0].v_string.str);
        qcc::String entityMac = lowercase(msg->GetArg(1)->v_struct.members[1].v_string.str);
        double rangeFromSensor = msg->GetArg(2)->v_double;
        //int64_t msgTimestamp = msg->GetTimeStamp();
        uint32_t msgTimestamp = ajn::BusAttachment::GetTimestamp();

        /* Post the information to the database. */
        lsDb->PostRangeDiscovery(rangeFromSensor,
                                 msgTimestamp,
                                 DbDevice(sensorUuid, sensorMac),
                                 DbDevice(entityUuid, entityMac));
        MethodReply(msg);

        //QCC_DbgPrintf(("EntityRange rangeFromSensor: %f\n", (double)rangeFromSensor));        

    } catch(std::exception& e) {
        QCC_DbgPrintf(("EntityRange Exception: %s\n", e.what())); 
    }
    injectMutex.Unlock();

    //QCC_DbgPrintf(("EntityRange - END"));

    return;
}

void Locationservices::PresenceSubscribe(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("PresenceSubscribe"));

    msgBus->EnableConcurrentCallbacks();
    
    /* Generate the tracker path. */
    qcc::String path = qcc::String(("/PresenceTracker/") + U32ToString(msg->GetTimeStamp()));;
    
    /* Create the tracker. */
    PresenceTracker* tracker = new PresenceTracker(path, msgBus, lsDb, lsMan);
    
    /* Start the tracker. */
    if (tracker != NULL) {
        QStatus status;

        trackerMutex.Lock(MUTEX_CONTEXT);
        try {
            if ((status = tracker->Start(msg)) == ER_OK) {
                presenceTrackers.push_back(tracker);
                
                MsgArg arg("o", tracker->GetPath());
                status = MethodReply(msg, &arg, 1);
            } else {
                delete tracker;
                status = MethodReply(msg, status);
            }
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("PresenceSubscribe Exception: %s\n", e.what())); 
        }
        trackerMutex.Unlock();
    }
    return;
}

void Locationservices::DistanceSubscribe(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("DistanceSubscribe"));

    msgBus->EnableConcurrentCallbacks();
    
    /* Generate the tracker path. */
    qcc::String path = qcc::String(("/DistanceTracker/") + U32ToString(msg->GetTimeStamp()));;
    
    /* Create the tracker. */
    DistanceTracker* tracker = new DistanceTracker(path, msgBus, lsDb, lsMan);
    
    /* Start the tracker. */
    if (tracker != NULL) {
        QStatus status;

        trackerMutex.Lock(MUTEX_CONTEXT);
        try {
            if ((status = tracker->Start(msg)) == ER_OK) {
                distanceTrackers.push_back(tracker);
                
                MsgArg arg("o", tracker->GetPath());
                status = MethodReply(msg, &arg, 1);
            } else {
                delete tracker;
                status = MethodReply(msg, status);
            }
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("DistanceSubscribe Exception: %s\n", e.what())); 
        }
        trackerMutex.Unlock();
    }
    return;
}
