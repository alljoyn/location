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
#include <ls/PositionContributorInterface.h>
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "PositionContributor.h"


using namespace ajn;
using namespace std;
using namespace qcc;


PositionContributor::PositionContributor(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb) 
    : ServiceObject(org_alljoyn_locationservices::position::Contributor::INTERFACE_PATH, msgBus, svcDb)
{
    QCC_DbgPrintf(("PositionContributor"));
    this->SetDescription("en", org_alljoyn_locationservices::position::Contributor::INTERFACE_DESCRIPTION);
}

PositionContributor::~PositionContributor()
{
    QCC_DbgPrintf(("~PositionContributor"));

    /* Unregister the bus object. */
    msgBus->UnregisterBusObject(*this);
}

QStatus PositionContributor::Start()
{
    QCC_DbgPrintf(("Start"));

    /* Create the interface. */
    QStatus status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::position::Contributor::INTERFACE_XML);
    
    /* Get the interface. */
    if (status == ER_OK) {
        svcIntf = msgBus->GetInterface(org_alljoyn_locationservices::position::Contributor::INTERFACE_NAME);
        status = (svcIntf != NULL) ? ER_OK : ER_FAIL;
    }

    /* Setup the method handlers. */
    if (status == ER_OK) {
        const MethodEntry handlers[] = {
            { svcIntf->GetMember("UpdatePositionInfo"), static_cast<MessageReceiver::MethodHandler>(&PositionContributor::UpdatePositionInfo) },
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

QStatus PositionContributor::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get"));

    QStatus status;

    try {
        if (!strcmp(ifcName, org_alljoyn_locationservices::position::Contributor::INTERFACE_NAME)) {
            if (strcmp(propName, "Version") == 0) {
                status = val.Set("q", org_alljoyn_locationservices::position::Contributor::INTERFACE_VERSION);

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

void PositionContributor::UpdatePositionInfo(const ajn::InterfaceDescription::Member* member, ajn::Message& msg) {
    QCC_DbgPrintf(("UpdatePositionInfo"));

    QStatus status = (svcIntf != NULL) ? ER_OK : ER_FAIL; 

    if (status == ER_OK) {
        try {
            msgBus->EnableConcurrentCallbacks();
                
            /* Get the message arguments. */
            qcc::String contributorUid = Lowercase(msg->GetArg(0)->v_struct.members[0].v_string.str);
            qcc::String contributorDescriptor = Lowercase(msg->GetArg(0)->v_struct.members[1].v_string.str);
            
            qcc::String discoveryUid = Lowercase(msg->GetArg(1)->v_struct.members[0].v_string.str);
            qcc::String discoveryDescriptor = Lowercase(msg->GetArg(1)->v_struct.members[1].v_string.str);
            
            Position position;
            position.origin = Lowercase(msg->GetArg(2)->v_struct.members[0].v_string.str);
            position.xCoordinate = msg->GetArg(2)->v_struct.members[1].v_double;
            position.yCoordinate = msg->GetArg(2)->v_struct.members[2].v_double;
            position.zCoordinate = msg->GetArg(2)->v_struct.members[3].v_double;
            double accuracy = msg->GetArg(2)->v_struct.members[4].v_double;
            
            /* Get the cursors for the devices. */
            bool cacheFlush = false;
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
            svcDb->InjectPresence(cacheFlush, contributorCursor, discoveryCursor, true);
            svcDb->InjectPosition(cacheFlush, contributorCursor, discoveryCursor, position, accuracy);

        } catch(std::exception& e) {
            QCC_DbgPrintf(("UpdatePositionInfo Exception: %s\n", e.what())); 
        }
    }
    MethodReply(msg);
}
