/******************************************************************************
 * Copyright (c) 2016, AllSeen Alliance. All rights reserved.
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
#include <ls/DistanceTrackerInterface.h>
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "TrackerObject.h"
#include "DistanceTracker.h"


using namespace ajn;
using namespace std;
using namespace qcc;


DistanceTracker::DistanceTracker(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb) 
    : TrackerObject(qcc::String(org_alljoyn_locationservices::distance::Tracker::INTERFACE_PATH), msgBus, svcDb),
      filterArg(NULL), deltaArg(0),
      attributeMutex(), attributeMap()
{
    QCC_DbgPrintf(("DistanceTracker"));
    this->SetDescription("en", org_alljoyn_locationservices::distance::Tracker::INTERFACE_DESCRIPTION);
}

DistanceTracker::~DistanceTracker()
{
    QCC_DbgPrintf(("~DistanceTracker"));

    /* Deregister listeners. */
    svcDb->DeregisterListener(this);

    /* Clear the attributes. */
    attributeMutex.Lock(MUTEX_CONTEXT);
    attributeMap.clear();
    attributeMutex.Unlock();

    /* Reclaim memory. */
    delete filterArg;

    /* Unregister the bus object. */
    msgBus->UnregisterBusObject(*this);
}

QStatus DistanceTracker::Start(ajn::Message& msg)
{
    QCC_DbgPrintf(("Start"));

    /* Create the interface. */
    QStatus status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::distance::Tracker::INTERFACE_XML);

    /* Setup the listeners. */
    if (status == ER_OK) {
        status = svcDb->RegisterListener(this);
    }

    /* Save off the subscription arguments and load the cache. */
    if (status == ER_OK) {
        matchesMutex.Lock(MUTEX_CONTEXT);
        try {
            const MsgArg* msgArg = msg->GetArg(0);
            if (msgArg != NULL) {
                filterArg = new MsgArg(*msgArg);

                DbCursor deviceCursor = svcDb->MapDevice(msgArg->v_struct.members[0].v_struct.members[0].v_string.str,
                                                         msgArg->v_struct.members[0].v_struct.members[1].v_string.str);
                DbCursor phraseCursor = svcDb->FindPhrase(msgArg->v_struct.members[0].v_struct.members[1].v_string.str);
                matchesCache = new DbCache(deviceCursor, phraseCursor);
                status = ConfigureFilter(1, 2, *filterArg, *matchesCache);
            } else {
                status = ER_BAD_ARG_COUNT;
            }
            if (status == ER_OK) {
                status = svcDb->CacheDistance(*matchesCache);
            }

        } catch(std::exception& e) {
            QCC_DbgPrintf(("Start Exception: %s\n", e.what()));
            status = ER_FAIL;
        }
        matchesMutex.Unlock();
    }
    if (status == ER_OK) {
        const MsgArg* msgArg = msg->GetArg(1);
        if (msgArg != NULL) {
            deltaArg = msgArg->v_double;
        } else {
            status = ER_BAD_ARG_COUNT;
        }
    }

    /* Get the interface. */
    if (status == ER_OK) {
        svcIntf = msgBus->GetInterface(org_alljoyn_locationservices::distance::Tracker::INTERFACE_NAME);
        status = (svcIntf != NULL) ? ER_OK : ER_FAIL;
    }
        
    /* Setup the method handlers. */
    if (status == ER_OK) {
        const MethodEntry handlers[] = {
            { svcIntf->GetMember("Matches"), static_cast<MessageReceiver::MethodHandler>(&DistanceTracker::Matches) },
            { svcIntf->GetMember("KeepAlive"), static_cast<MessageReceiver::MethodHandler>(&DistanceTracker::KeepAlive) },
        };
        status = AddInterface(*svcIntf, ANNOUNCED);
        if (status == ER_OK) {
            status = AddMethodHandlers(handlers, ArraySize(handlers));
        }
    }

    /* Register the bus object. */
    if (status == ER_OK) {
        status = msgBus->RegisterBusObject(*this);
    }
    
    QCC_DbgPrintf(("Start: %s", QCC_StatusText(status)));
 
    return status;
}

void DistanceTracker::DistanceEvent(bool cacheFlush,
                                    DbCursor contributorCursor, 
                                    DbCursor discoveryCursor,
                                    double distance)
{
    QCC_DbgPrintf(("DistanceEvent"));

    if (svcIntf != NULL) {
        SignalChanges(cacheFlush, contributorCursor, discoveryCursor, distance);
    }
}

QStatus DistanceTracker::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get"));

    QStatus status;

    try {
        if (!strcmp(ifcName, org_alljoyn_locationservices::distance::Tracker::INTERFACE_NAME)) {
            if (strcmp(propName, "Version") == 0) {
                status = val.Set("q", org_alljoyn_locationservices::distance::Tracker::INTERFACE_VERSION);

            } else if (strcmp(propName, "Delta") ==0) {
                status = val.Set("d", deltaArg);

            } else if (strcmp(propName, "Filter") == 0) {
                status = (filterArg != NULL) 
                    ? val.Set("(ss)(ss)a(ss)",
                              filterArg->v_struct.members[0].v_struct.members[0].v_string.str,
                              filterArg->v_struct.members[0].v_struct.members[1].v_string.str,
                              filterArg->v_struct.members[1].v_struct.members[0].v_string.str,
                              filterArg->v_struct.members[1].v_struct.members[1].v_string.str,
                              (int)(filterArg->v_struct.members[2].v_array.GetNumElements()),
                              filterArg->v_struct.members[2].v_array.GetElements())
                    : ER_FAIL;

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

    QCC_DbgPrintf(("Get: %s", QCC_StatusText(status)));
    return status;
}

void DistanceTracker::Matches(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("Matches"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 
    MsgArg* replyArg = NULL;

    matchesMutex.Lock(MUTEX_CONTEXT);
    try {
        msgBus->EnableConcurrentCallbacks();

        /* Setup the reply information. */
        if (status == ER_OK) {
            replyArg = new MsgArg();
            status = svcDb->DistanceEntities(*matchesCache, *replyArg);
        }

    } catch(std::exception& e) {
        status = ER_FAIL;
        QCC_DbgPrintf(("Matches Exception: %s\n", e.what())); 
    }
    matchesMutex.Unlock();

    /* Send the reply. */
    if (status == ER_OK) {
        MethodReply(msg, replyArg, 1);
    } else {
        MethodReply(msg, status);
    }
    delete replyArg;
}

void DistanceTracker::KeepAlive(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("KeepAlive"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 

    try {
        msgBus->EnableConcurrentCallbacks();
        
        if (status == ER_OK) {
            LifetimeExtension();
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("KeepAlive Exception: %s\n", e.what())); 
    }
    MethodReply(msg);
}

void DistanceTracker::SignalChanges(bool cacheFlush,
                                    DbCursor contributorCursor, 
                                    DbCursor discoveryCursor,
                                    double distance)
{
    QCC_DbgPrintf(("SignalChanges"));

    matchesMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Find the device cursor we are interested in. */
        DbCursor deviceCursor;
        if (contributorCursor == matchesCache->GetRefDevice()) {
            deviceCursor = discoveryCursor;
        } else if (discoveryCursor == matchesCache->GetRefDevice()) {
            deviceCursor = contributorCursor;
        } else {
            deviceCursor = 0;
        }
        
        /* Handle cache updates and signalling only if we have a cursor match. */
        if (svcDb->CacheMatch(deviceCursor, *matchesCache)) {
            /* Refresh the cache if necessary. */
            if (cacheFlush) {
                svcDb->CacheRefresh(deviceCursor, *matchesCache);
            }
            
            /* Issue a signal for the changed cursors only if the attribute is changed. */
            if (PostAttribute(deviceCursor, distance)) {
                std::vector<DbEntity>::iterator it;
                for (int position = 0; position < (int)matchesCache->entities.size(); position++ ) {
                    DbEntity& entity = matchesCache->entities.at(position);
                    if (entity.deviceCursor == deviceCursor) {
                        MsgArg msgArgs[] = { {MsgArg("o", ServiceObject::GetPath())},
                                             {MsgArg()} };
                        svcDb->DistanceEntity(position, *matchesCache, msgArgs[1]);
                        msgArgs[1].v_struct.members[1].v_double = distance;
                        
                        ServiceObject::Signal(NULL, 
                                              SESSION_ID_ALL_HOSTED, 
                                              *(svcIntf->GetMember("EntityChanged")), 
                                              msgArgs, 
                                              ArraySize(msgArgs));
                    }
                }
            }
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("SignalChanges Exception: %s\n", e.what()));
    }
    matchesMutex.Unlock();
}

bool DistanceTracker::PostAttribute(DbCursor cursor, double distance) 
{
    bool result;

    attributeMutex.Lock(MUTEX_CONTEXT);    
    std::unordered_map<DbCursor, double>::iterator it = attributeMap.find(cursor);
    if (it != attributeMap.end()) {
        result = (std::fabs(distance - it->second) >= std::fabs(deltaArg));
        if (result) {
            attributeMap.erase(cursor);
            attributeMap.emplace(cursor, distance);
        }
    } else {
        result = true;
        attributeMap.emplace(cursor, distance);
    }
    attributeMutex.Unlock();

    return result;
}


