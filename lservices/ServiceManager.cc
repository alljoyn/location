/******************************************************************************
 * Copyright (c) 2016 Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright 2016 Open Connectivity Foundation and Contributors to
 *    AllSeen Alliance. All rights reserved.
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
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "TrackerObject.h"
#include "ServiceManager.h"
#include "PresenceContributor.h"
#include "DistanceContributor.h"
#include "PositionContributor.h"
#include "PresenceManager.h"
#include "DistanceManager.h"
#include "GeofenceManager.h"
#include "PositionManager.h"


using namespace ajn;
using namespace std;
using namespace qcc;


static const char* svcJoiner = NULL;
static ajn::SessionPort svcPort = ajn::SESSION_PORT_ANY;
//static ajn::SessionPort svcPort = 1234;
//const ajn::TransportMask svcTransport = ajn::TRANSPORT_TCP;
const ajn::TransportMask svcTransport = ajn::TRANSPORT_ANY;


QStatus ServiceManager::ApiInclude(ServiceObject* svcObject) {
    QStatus status;

    if (svcObject != NULL) {
        status = svcObject->Start();
        if (status == ER_OK) {
            svcObjectsMutex.Lock(MUTEX_CONTEXT);
            svcObjects.push_back(svcObject);
            svcObjectsMutex.Unlock();
        }
    } else {
        status = ER_FAIL;
    }
    return status;
}

bool ServiceManager::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const SessionOpts& opts) 
{
    QCC_DbgPrintf(("AcceptSessionJoiner sessionPort %d, joiner %s, opts %s", sessionPort, joiner, opts.ToString().c_str()));

    bool result = (sessionPort == svcPort);
    return( result );
}

void ServiceManager::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner)
{
    QCC_DbgPrintf(("SessionJoined sessionPort %d, id %d, joiner %s", sessionPort, id, joiner));
    svcJoiner = joiner;
}

ServiceManager::ServiceManager(ajn::BusAttachment* msgBus) 
        :  msgBus(msgBus), aboutData("en"), aboutObj(NULL), svcDb(new ServiceDatabase()),
           svcObjectsMutex(), svcObjects(), apiTimestamp(0)
{
    aboutData.SetAppId(APP_GUID);
    aboutData.SetDefaultLanguage("en");
    aboutData.SetSupportedLanguage("en");
    aboutData.SetDeviceName("Location Services Provider", "en");
    aboutData.SetDeviceId(DEVICE_GUID);
    aboutData.SetAppName("alljoyn.locationservices", "en");
    aboutData.SetManufacturer("Liquid Motion", "en");
    aboutData.SetModelNumber("123456");
    aboutData.SetDescription("Locationservices Discovery And Notification", "en");
    aboutData.SetSoftwareVersion("0.0.1");
    assert(aboutData.IsValid());
}

ServiceManager::~ServiceManager()
{
    QCC_DbgPrintf(("~ServiceManager"));

    /* Release the well-known msg bus name. */
    msgBus->ReleaseName(SERVICE_NAME);

    /* Delete the service objects. */
    svcObjectsMutex.Lock(MUTEX_CONTEXT);
    for (vector<ServiceObject*>::iterator it = svcObjects.begin(); it != svcObjects.end(); ++it) {
        delete *it;
    }
    svcObjectsMutex.Unlock(); 
        
    /* Reclaim memory. */
    delete aboutObj;
    delete svcDb;
}

QStatus ServiceManager::StartApi()
{
    QStatus status = (msgBus != NULL) ? ER_OK : ER_FAIL;

    /* Start the message bus. */
    if (status == ER_OK) {
        status = msgBus->Start();
    }

    /* Include the Presence Contributor in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new PresenceContributor(msgBus, svcDb));
    }

    /* Include the Distance Contributor in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new DistanceContributor(msgBus, svcDb));
    }

    /* Include the Position Contributor in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new PositionContributor(msgBus, svcDb));
    }

    /* Include the Presence Manager in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new PresenceManager(msgBus, svcDb));
    }

    /* Include the Distance Manager in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new DistanceManager(msgBus, svcDb));
    }

    /* Include the Geofence Manager in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new GeofenceManager(msgBus, svcDb));
    }

    /* Include the Position Manager in the API. */
    if (status == ER_OK) {
        status = ApiInclude(new PositionManager(msgBus, svcDb));
    }

    /* Locate and connect to a router. */
    if (status == ER_OK) {
        status = msgBus->Connect();
    }

    /* Request a service name. */
    if (status == ER_OK) {
	    const uint32_t flags = DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE;
	    status = msgBus->RequestName(SERVICE_NAME, flags);
    }

    /* Make the message bus available for use. */
    ajn::SessionOpts opts( SessionOpts::TrafficType::TRAFFIC_MESSAGES,
                           true,
                           SessionOpts::PROXIMITY_ANY,
                           svcTransport );

    if (status == ER_OK) {
        status = msgBus->BindSessionPort(svcPort, opts, *static_cast<SessionPortListener*>(this));
    }

    /* Advertise the service. */
    if (status == ER_OK) {
        status = msgBus->AdvertiseName(SERVICE_NAME, svcTransport);
    }

    /* Announce the services. */
    if (status == ER_OK) {
        aboutObj = new AboutObj(*msgBus);
        status = ((aboutData.IsValid("en")) && (aboutObj != NULL)) 
            ? aboutObj->Announce(svcPort, aboutData) 
            : ER_FAIL;
    }
   
    /* Set any timestamps. */
    if (status == ER_OK) {
        apiTimestamp = ajn::BusAttachment::GetTimestamp();
    }
 
    QCC_DbgPrintf(("StartApi: %s", QCC_StatusText(status)));
 
    return status;
}

void ServiceManager::CheckApi() 
{
    uint32_t currTimestamp = ajn::BusAttachment::GetTimestamp();

    if ((currTimestamp - apiTimestamp) >= 15000) {
        apiTimestamp = currTimestamp;

        for (vector<ServiceObject*>::iterator it = svcObjects.begin(); it != svcObjects.end(); ++it) {
            ServiceObject* svcObject = *it;
            svcObject->Check();
        }
    }
}




