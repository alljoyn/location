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
#include <ls/GeofenceManagerInterface.h>
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "TrackerObject.h"
#include "GeofenceTracker.h"
#include "GeofenceManager.h"


using namespace ajn;
using namespace std;
using namespace qcc;


GeofenceManager::GeofenceManager(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb) 
    : ServiceObject(org_alljoyn_locationservices::geofence::Manager::INTERFACE_PATH, msgBus, svcDb),
      trackersMutex(), trackers()
{
    QCC_DbgPrintf(("GeofenceManager"));
    this->SetDescription("en", org_alljoyn_locationservices::geofence::Manager::INTERFACE_DESCRIPTION);
}

GeofenceManager::~GeofenceManager()
{
    QCC_DbgPrintf(("~GeofenceManager"));

    /* Delete the trackers. */
    trackersMutex.Lock(MUTEX_CONTEXT);
    for (vector<TrackerObject*>::iterator it = trackers.begin(); it != trackers.end(); ++it) {
        delete *it;
    }
    trackers.clear();
    trackersMutex.Unlock();

    /* Unregister the bus object. */
    msgBus->UnregisterBusObject(*this);
}

QStatus GeofenceManager::Start()
{
    QCC_DbgPrintf(("Start"));

    /* Create the interface. */
    QStatus status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::geofence::Manager::INTERFACE_XML);
    
    /* Get the interface. */
    if (status == ER_OK) {
        svcIntf = msgBus->GetInterface(org_alljoyn_locationservices::geofence::Manager::INTERFACE_NAME);
        status = (svcIntf != NULL) ? ER_OK : ER_FAIL;
    }

    /* Setup the method handlers. */
    if (status == ER_OK) {
        const MethodEntry handlers[] = {
            { svcIntf->GetMember("GeofenceSubscribe"), static_cast<MessageReceiver::MethodHandler>(&GeofenceManager::GeofenceSubscribe) },
            { svcIntf->GetMember("QueryGeofence"), static_cast<MessageReceiver::MethodHandler>(&GeofenceManager::QueryGeofence) },
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

void GeofenceManager::Check() 
{
    trackersMutex.Lock(MUTEX_CONTEXT);
    for (vector<TrackerObject*>::iterator it = trackers.begin(); it != trackers.end(); ) {
        TrackerObject* tracker = *it;
        if ((tracker != NULL) && (!tracker->LifetimeRemaining())) {
            QCC_DbgPrintf(("Ending Tracker - %s", tracker->GetPath()));
            it = trackers.erase(it);
            delete tracker;
        } else {
            ++it;
        }
    }
    trackersMutex.Unlock();
}

QStatus GeofenceManager::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get"));

    QStatus status;

    try {
        if (!strcmp(ifcName, org_alljoyn_locationservices::geofence::Manager::INTERFACE_NAME)) {
            if (strcmp(propName, "Version") == 0) {
                status = val.Set("q", org_alljoyn_locationservices::geofence::Manager::INTERFACE_VERSION);

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

void GeofenceManager::GeofenceSubscribe(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("GeofenceSubscribe"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 

    try {
        msgBus->EnableConcurrentCallbacks();

        /* Create the tracker. */
        TrackerObject* tracker = NULL;
        if (status == ER_OK) {
            tracker = new GeofenceTracker(msgBus, svcDb);
            if (tracker == NULL) {
                status = ER_FAIL;
            }
        }
        
        /* Start the tracker. */
        if (status == ER_OK) {
            status = tracker->Start(msg);
        }

        /* Send the reply. */
        if (status == ER_OK) {
            MsgArg msgArgs[] = { {MsgArg("o", tracker->GetPath())} };
            MethodReply(msg, msgArgs, ArraySize(msgArgs));

            trackersMutex.Lock(MUTEX_CONTEXT);
            trackers.push_back(tracker);
            trackersMutex.Unlock();
        } else {
            delete tracker;
            MethodReply(msg, status);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("GeofenceSubscribe Exception: %s\n", e.what())); 
    }
    return;
}

void GeofenceManager::QueryGeofence(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("QueryGeofence"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 

    double fenceline = 0;
    DbCache* cache = NULL;
    MsgArg* replyArg = NULL;
    try {
        msgBus->EnableConcurrentCallbacks();

        /* Get the specified fenceline.  Setup the cache/filter for geofence entities. */
        if (status == ER_OK) {
            const MsgArg* msgArg = msg->GetArg(0);
            if (msgArg != NULL) {
                fenceline = msgArg->v_struct.members[0].v_double;
                DbCursor deviceCursor = svcDb->FindDevice(msgArg->v_struct.members[1].v_struct.members[0].v_string.str,
                                                          msgArg->v_struct.members[1].v_struct.members[1].v_string.str);
                DbCursor phraseCursor = svcDb->FindPhrase(msgArg->v_struct.members[1].v_struct.members[1].v_string.str);
                cache = new DbCache(deviceCursor, phraseCursor);
                status = ConfigureFilter(2, 3, *msgArg, *cache);
            } else {
                status = ER_BAD_ARG_COUNT;
            }
        } 

        /* Cache the matching distance information. */
        if (status == ER_OK) {
            status = svcDb->CacheDistance(*cache);
        }

        /* Setup the reply information. */
        if (status == ER_OK) {
            replyArg = new MsgArg();
            status = svcDb->GeofenceEntities(fenceline, *cache, *replyArg);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("QueryGeofence Exception: %s\n", e.what())); 
    }

    /* Send the reply. */
    if (status == ER_OK) {
        MethodReply(msg, replyArg, 1);
    } else {
        MethodReply(msg, status);
    }
    delete replyArg;
    delete cache;
}
