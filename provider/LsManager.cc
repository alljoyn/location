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


static const char* mgmtJoiner = NULL;
//static ajn::SessionPort mgmtPort = ajn::SESSION_PORT_ANY;
static ajn::SessionPort mgmtPort = 1234;
//const ajn::TransportMask mgmtTransport = ajn::TRANSPORT_TCP;
const ajn::TransportMask mgmtTransport = ajn::TRANSPORT_ANY;


LsManager::LsManager(ajn::BusAttachment* msgBus) 
        :  msgBus(msgBus), aboutData("en"), aboutObj(NULL)
{
    lsDb = new LsDatabase();

    aboutData.SetAppId(GUID);
    aboutData.SetDefaultLanguage("en");
    aboutData.SetSupportedLanguage("en");
    aboutData.SetDeviceName("Locationservices Hub", "en");
    aboutData.SetDeviceId("1f9a85a2-a519-11e5-bf7f-feff819cdc9f");
    //aboutData.SetDeviceId("26144417-c00b-414a-a34f-d96b01234e56");
    aboutData.SetAppName("alljoyn.locationservices", "en");
    aboutData.SetManufacturer("Liquid Motion", "en");
    aboutData.SetModelNumber("123456");
    aboutData.SetDescription("Locationservices Discovery And Notification", "en");
    //aboutData.SetDateOfManufacture("2015-07-21");
    aboutData.SetSoftwareVersion("0.0.1");
    //aboutData.SetHardwareVersion("0.0.1");
    //aboutData.SetSupportUrl("http://www.alljoyn.org");
    assert(aboutData.IsValid());
}

LsManager::~LsManager()
{
    QCC_DbgPrintf(("~LsManager"));

    /* Release the well-known msg bus name. */
    msgBus->ReleaseName("org.alljoyn.locationservices");
        
    /* Reclaim memory. */
    delete aboutObj;
    delete lsObject;
}

QStatus LsManager::StartApi()
{
    QStatus status = (msgBus != NULL) ? ER_OK : ER_FAIL;

    /* Start the message bus. */
    if (status == ER_OK) {
        status = msgBus->Start();
    }

    /* Create the remaining interfaces of interest on the bus. */
    if (status == ER_OK) {
        status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::Locationservices::InterfaceXml);
    }
    if (status == ER_OK) {
        status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::PresenceTracker::InterfaceXml);
    }
    if (status == ER_OK) {
        status = msgBus->CreateInterfacesFromXml(org_alljoyn_locationservices::DistanceTracker::InterfaceXml);
    }

    /* Start the Locationservices API. */
    if (status == ER_OK) {
        lsObject = new Locationservices(msgBus, lsDb, this);
        status = lsObject->Start();
    }

    /* Locate and connect to a router. */
    if (status == ER_OK) {
        status = msgBus->Connect();
    }

    /* Request a service name. */
    if (status == ER_OK) {
	    const uint32_t flags = DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE;
	    status = msgBus->RequestName("org.alljoyn.locationservices", flags);
    }

    /* Make the message bus available for use. */
    ajn::SessionOpts opts( SessionOpts::TrafficType::TRAFFIC_MESSAGES,
                           true,
                           SessionOpts::PROXIMITY_ANY,
                           mgmtTransport );

    if (status == ER_OK) {
        status = msgBus->BindSessionPort(mgmtPort, opts, *static_cast<SessionPortListener*>(this));
    }

    /* Advertise the service. */
    if (status == ER_OK) {
        status = msgBus->AdvertiseName(org_alljoyn_locationservices::ServiceName, mgmtTransport);
    }

    /* Announce the services. */
    if (status == ER_OK) {
        aboutObj = new AboutObj(*msgBus);
        status = ((aboutData.IsValid("en")) && (aboutObj != NULL)) 
            ? aboutObj->Announce(mgmtPort, aboutData) 
            : ER_FAIL;
    }
   
    /* Set any timestamps. */
    if (status == ER_OK) {
        apiTimestamp = ajn::BusAttachment::GetTimestamp();
    }
 
    QCC_DbgPrintf(("StartApi: %s", QCC_StatusText(status)));
 
    return status;
}

void LsManager::CheckApi() 
{
    uint32_t currTimestamp = ajn::BusAttachment::GetTimestamp();

    if ((currTimestamp - apiTimestamp) >= 15000) {
        apiTimestamp = currTimestamp;

        if (lsObject != NULL) {
            lsObject->Check();
        }
    }
}

bool LsManager::AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const SessionOpts& opts) 
{
    QCC_DbgPrintf(("AcceptSessionJoiner sessionPort %d, joiner %s, opts %s", sessionPort, joiner, opts.ToString().c_str()));

    bool result = (sessionPort == mgmtPort);
    return( result );
}

void LsManager::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner)
{
    QCC_DbgPrintf(("SessionJoined sessionPort %d, id %d, joiner %s", sessionPort, id, joiner));
    mgmtJoiner = joiner;
}





