/******************************************************************************
 * Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *     WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *     WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *     AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *     DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *     PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *     TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *     PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#include <ls/GeofenceTrackerInterface.h>
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "TrackerObject.h"
#include "GeofenceTracker.h"


using namespace ajn;
using namespace std;
using namespace qcc;


GeofenceTracker::GeofenceTracker(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb) 
    : TrackerObject(qcc::String(org_alljoyn_locationservices::geofence::Tracker::INTERFACE_PATH), msgBus, svcDb),
      filterArg(NULL), fencelineArg(0),
      attributeMutex(), attributeMap()
{
    QCC_DbgPrintf(("GeofenceTracker"));
    this->SetDescription("en", org_alljoyn_locationservices::geofence::Tracker::INTERFACE_DESCRIPTION);
}

GeofenceTracker::~GeofenceTracker()
{
    QCC_DbgPrintf(("~GeofenceTracker"));

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

QStatus GeofenceTracker::Start(ajn::Message& msg)
{
    QCC_DbgPrintf(("Start"));

    /* Create the interface. */
    QStatus status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::geofence::Tracker::INTERFACE_XML);

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
                fencelineArg = msgArg->v_struct.members[0].v_double;

                filterArg = new MsgArg(*msgArg);

                DbCursor deviceCursor = svcDb->MapDevice(msgArg->v_struct.members[1].v_struct.members[0].v_string.str,
                                                         msgArg->v_struct.members[1].v_struct.members[1].v_string.str);
                DbCursor phraseCursor = svcDb->FindPhrase(msgArg->v_struct.members[1].v_struct.members[1].v_string.str);
                matchesCache = new DbCache(deviceCursor, phraseCursor);
                status = ConfigureFilter(2, 3, *filterArg, *matchesCache);
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

    /* Get the interface. */
    if (status == ER_OK) {
        svcIntf = msgBus->GetInterface(org_alljoyn_locationservices::geofence::Tracker::INTERFACE_NAME);
        status = (svcIntf != NULL) ? ER_OK : ER_FAIL;
    }
        
    /* Setup the method handlers. */
    if (status == ER_OK) {
        const MethodEntry handlers[] = {
            { svcIntf->GetMember("Matches"), static_cast<MessageReceiver::MethodHandler>(&GeofenceTracker::Matches) },
            { svcIntf->GetMember("KeepAlive"), static_cast<MessageReceiver::MethodHandler>(&GeofenceTracker::KeepAlive) },
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

void GeofenceTracker::DistanceEvent(bool cacheFlush,
                                    DbCursor contributorCursor, 
                                    DbCursor discoveryCursor,
                                    double distance)
{
    QCC_DbgPrintf(("DistanceEvent"));

    if (svcIntf != NULL) {
        SignalChanges(cacheFlush, contributorCursor, discoveryCursor, distance);
    }
}

QStatus GeofenceTracker::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get"));

    QStatus status;

    try {
        if (!strcmp(ifcName, org_alljoyn_locationservices::geofence::Tracker::INTERFACE_NAME)) {
            if (strcmp(propName, "Version") == 0) {
                status = val.Set("q", org_alljoyn_locationservices::geofence::Tracker::INTERFACE_VERSION);

            } else if (strcmp(propName, "Filter") == 0) {
                status = (filterArg != NULL) 
                    ? val.Set("d(ss)(ss)a(ss)",
                              filterArg->v_struct.members[0].v_double,
                              filterArg->v_struct.members[1].v_struct.members[0].v_string.str,
                              filterArg->v_struct.members[1].v_struct.members[1].v_string.str,
                              filterArg->v_struct.members[2].v_struct.members[0].v_string.str,
                              filterArg->v_struct.members[2].v_struct.members[1].v_string.str,
                              (int)(filterArg->v_struct.members[3].v_array.GetNumElements()),
                              filterArg->v_struct.members[3].v_array.GetElements())
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

void GeofenceTracker::Matches(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
        QCC_DbgPrintf(("Matches - %f", fencelineArg));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 
    MsgArg* replyArg = NULL;

    matchesMutex.Lock(MUTEX_CONTEXT);
    try {
        msgBus->EnableConcurrentCallbacks();

        /* Setup the reply information. */
        if (status == ER_OK) {
            replyArg = new MsgArg();
            status = svcDb->GeofenceEntities(fencelineArg, *matchesCache, *replyArg);
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

void GeofenceTracker::KeepAlive(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
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

void GeofenceTracker::SignalChanges(bool cacheFlush,
                                    DbCursor contributorCursor, 
                                    DbCursor discoveryCursor,
                                    double distance)
{
        QCC_DbgPrintf(("SignalChanges - %f", fencelineArg));

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
            bool withinFenceline = (std::fabs(distance) <= std::fabs(fencelineArg));
            if (PostAttribute(deviceCursor, withinFenceline)) {
                std::vector<DbEntity>::iterator it;
                for (int position = 0; position < (int)matchesCache->entities.size(); position++ ) {
                    DbEntity& entity = matchesCache->entities.at(position);
                    if (entity.deviceCursor == deviceCursor) {
                        MsgArg msgArgs[] = { {MsgArg("o", ServiceObject::GetPath())},
                                             {MsgArg()} };
                        svcDb->GeofenceEntity(withinFenceline, entity, msgArgs[1]);
                        
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

bool GeofenceTracker::PostAttribute(DbCursor cursor, bool withinFenceline) 
{
    bool result;

    attributeMutex.Lock(MUTEX_CONTEXT);    
    std::unordered_map<DbCursor, bool>::iterator it = attributeMap.find(cursor);
    if (it != attributeMap.end()) {
        if ((result = (it->second != withinFenceline))) {
            attributeMap.erase(cursor);
            attributeMap.emplace(cursor, withinFenceline);
        }
    } else {
        result = true;
        attributeMap.emplace(cursor, withinFenceline);
    }
    attributeMutex.Unlock();

    return result;
}

