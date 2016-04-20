/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
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
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#include <ls/PresenceContributorInterface.h>
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "PresenceContributor.h"


using namespace ajn;
using namespace std;
using namespace qcc;


PresenceContributor::PresenceContributor(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb) 
    : ServiceObject(org_alljoyn_locationservices::presence::Contributor::INTERFACE_PATH, msgBus, svcDb),
      limitFilterMutex(), limitFilter(NULL)
{
    QCC_DbgPrintf(("PresenceContributor"));
    this->SetDescription("en", org_alljoyn_locationservices::presence::Contributor::INTERFACE_DESCRIPTION);
}

PresenceContributor::~PresenceContributor()
{
    QCC_DbgPrintf(("~PresenceContributor"));

    /* Delete any limit filter. */
    limitFilterMutex.Lock(MUTEX_CONTEXT);
    delete limitFilter;
    limitFilter = NULL;
    limitFilterMutex.Unlock();

    /* Unregister the bus object. */
    msgBus->UnregisterBusObject(*this);
}

QStatus PresenceContributor::Start()
{
    QCC_DbgPrintf(("Start"));

    /* Create the interface. */
    QStatus status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::presence::Contributor::INTERFACE_XML);
    
    /* Get the interface. */
    if (status == ER_OK) {
        svcIntf = msgBus->GetInterface(org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME);
        status = (svcIntf != NULL) ? ER_OK : ER_FAIL;
    }

    /* Setup the method handlers. */
    if (status == ER_OK) {
        const MethodEntry handlers[] = {
            { svcIntf->GetMember("UpdatePresenceInfo"), static_cast<MessageReceiver::MethodHandler>(&PresenceContributor::UpdatePresenceInfo) },
            { svcIntf->GetMember("LimitContribution"), static_cast<MessageReceiver::MethodHandler>(&PresenceContributor::LimitContribution) },
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

QStatus PresenceContributor::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get"));

    QStatus status;

    try {
        if (!strcmp(ifcName, org_alljoyn_locationservices::presence::Contributor::INTERFACE_NAME)) {
            if (strcmp(propName, "Version") == 0) {
                status = val.Set("q", org_alljoyn_locationservices::presence::Contributor::INTERFACE_VERSION);

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

void PresenceContributor::UpdatePresenceInfo(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("UpdatePresenceInfo"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 

    if (status == ER_OK) {
        try {
            msgBus->EnableConcurrentCallbacks();

            /* Get the message arguments. */
            const qcc::String contributorUid = Lowercase(msg->GetArg(0)->v_struct.members[0].v_string.str);
            const qcc::String contributorDescriptor = Lowercase(msg->GetArg(0)->v_struct.members[1].v_string.str);
            
            const qcc::String discoveryUid = Lowercase(msg->GetArg(1)->v_struct.members[0].v_string.str);
            const qcc::String discoveryDescriptor = Lowercase(msg->GetArg(1)->v_struct.members[1].v_string.str);
            
            bool isPresent = msg->GetArg(2)->v_bool;

            /* Inject the information. */
            if ((limitFilter != NULL) && (limitFilter->IsMatch(discoveryUid, discoveryDescriptor))) {
                bool cacheFlush = false;

                /* Get the cursors for the devices. */
                DbCursor contributorCursor = svcDb->FindDevice(contributorUid, contributorDescriptor);
                if (contributorCursor == 0) {
                    cacheFlush = true;
                    contributorCursor = svcDb->MapDevice(contributorUid, contributorDescriptor);
                }
                DbCursor discoveryCursor = svcDb->FindDevice(discoveryUid, discoveryDescriptor);
                if (discoveryCursor == 0) {
                    cacheFlush = true;
                    discoveryCursor = svcDb->MapDevice(discoveryUid, discoveryDescriptor);
                }
                
                /* Post the information to the database. */
                svcDb->InjectPresence(cacheFlush, contributorCursor, discoveryCursor, isPresent);
            }

        } catch(std::exception& e) {
            QCC_DbgPrintf(("UpdatePresenceInfo Exception: %s\n", e.what())); 
        }
    }
    MethodReply(msg);
}

void PresenceContributor::LimitContribution(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("LimitContribution"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 

    if (status == ER_OK) {
        limitFilterMutex.Lock(MUTEX_CONTEXT);
        try {
            msgBus->EnableConcurrentCallbacks();

            /* Delete the old filter. */
            delete limitFilter;
            limitFilter = NULL;

            /* Setup the general limit filter for presence entities. */    
            limitFilter = new DbFilter(0, 0);
            const MsgArg* msgArg = msg->GetArg(0);
            if (msgArg != NULL) {
                ConfigureFilter(0, 1, *msgArg, *limitFilter); 
            }

        } catch(std::exception& e) {
            QCC_DbgPrintf(("LimitContribution Exception: %s\n", e.what())); 
        }
        limitFilterMutex.Unlock();
    }
    MethodReply(msg);
}
