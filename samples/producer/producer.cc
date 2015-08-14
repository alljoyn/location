/**
 * @file
 * @brief
 */

/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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

#include <alljoyn/AboutObj.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>
#include <alljoyn/location/LocationStd.h>
#include <alljoyn/version.h>

#include <qcc/String.h>
#include <qcc/StringUtil.h>

#include <algorithm>
#include <stdio.h>
#include <signal.h>

using namespace ajn;
using namespace qcc;
using namespace std;

static volatile sig_atomic_t g_interrupt = false;

static void CDECL_CALL SigIntHandler(int)
{
    g_interrupt = true;
}

static char* GetLine(char*str, size_t num, FILE* fp)
{
    char* p = fgets(str, num, fp);
    if (p != NULL) {
        size_t last = strlen(str) - 1;
        if (str[last] == '\n') {
            str[last] = '\0';
        }
    }
    return p;
}

static String NextTok(String& inStr)
{
    String ret;
    size_t off = inStr.find_first_of(' ');
    if (off == String::npos) {
        ret = inStr;
        inStr.clear();
    } else {
        ret = inStr.substr(0, off);
        inStr = Trim(inStr.substr(off));
    }
    return Trim(ret);
}

static QStatus CreateInterfaces(BusAttachment* bus)
{
    QStatus status = bus->CreateInterfacesFromXml(org::alljoyn::locationservices::InterfaceXml);
    if (status == ER_OK) {
        status = bus->CreateInterfacesFromXml(org::alljoyn::locationservices::PresenceTracker::InterfaceXml);
    }
    if (status == ER_OK) {
        status = bus->CreateInterfacesFromXml(org::alljoyn::locationservices::DistanceTracker::InterfaceXml);
    }
    if (status == ER_OK) {
        status = bus->CreateInterfacesFromXml(org::alljoyn::locationservices::GeofenceTracker::InterfaceXml);
    }
    if (status == ER_OK) {
        status = bus->CreateInterfacesFromXml(org::alljoyn::locationservices::LocationTracker::InterfaceXml);
    }
    return status;
}

struct Entity {
    qcc::String id;
    uint8_t mac[6];
    Entity(const qcc::String& id, const uint8_t mac[6]) : id(id) { memcpy(this->mac, mac, 6); }
    bool operator==(const Entity& other) { return (id == other.id) && !memcmp(mac, other.mac, 6); }
};

typedef std::vector<Entity> Matches;

class Producer;

class PresenceTrackerObj : public BusObject {
  public:
    PresenceTrackerObj(Producer* producer, const char* path);
    virtual ~PresenceTrackerObj() { }
    QStatus Register(BusAttachment* bus);
  private:
    Producer* m_producer;
    uint16_t m_version;
    virtual QStatus Get(const char* ifcName, const char* propName, ajn::MsgArg& val);
    QStatus MatchesChanged();
};

class Producer : public BusObject, public SessionPortListener {
  public:
    Producer();
    virtual ~Producer();
    QStatus Start(BusAttachment* bus);
    QStatus Add(const Entity& entity);
    QStatus Remove(const Entity& entity);
    const Matches& GetMatches();
  private:
    BusAttachment* m_bus;
    AboutData m_aboutData;
    AboutObj* m_aboutObj;
    Matches m_matches;
    vector<PresenceTrackerObj*> m_presenceTrackers;
    virtual bool AcceptSessionJoiner(SessionPort, const char*, const SessionOpts&) { return true; }
    void PresenceSubscribe(const InterfaceDescription::Member* member, Message& msg);
    void MatchesChanged();
};

Producer::Producer()
    : BusObject("/")
    , m_bus(NULL)
    , m_aboutData("en")
    , m_aboutObj(NULL)
{
    const uint8_t appId[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    m_aboutData.SetAppId(appId, 16);
    m_aboutData.SetDeviceName("DeviceName");
    m_aboutData.SetDeviceId("DeviceId");
    m_aboutData.SetAppName("AppName");
    m_aboutData.SetManufacturer("Manufacturer");
    m_aboutData.SetModelNumber("123456");
    m_aboutData.SetDescription("Description");
    m_aboutData.SetDateOfManufacture("2015-07-21");
    m_aboutData.SetSoftwareVersion("0.0.1");
    m_aboutData.SetHardwareVersion("0.0.1");
    m_aboutData.SetSupportUrl("http://www.alljoyn.org");
    assert(m_aboutData.IsValid());
}

Producer::~Producer()
{
    for (vector<PresenceTrackerObj*>::iterator it = m_presenceTrackers.begin(); it != m_presenceTrackers.end(); ++it) {
        delete *it;
    }
    delete m_aboutObj;
    m_bus->UnregisterBusObject(*this);
}

QStatus Producer::Start(BusAttachment* bus)
{
    m_bus = bus;
    const InterfaceDescription* intf = m_bus->GetInterface(org::alljoyn::locationservices::InterfaceName);
    assert(intf);
    const MethodEntry handlers[] = {
        { intf->GetMember("PresenceSubscribe"), static_cast<MessageReceiver::MethodHandler>(&Producer::PresenceSubscribe) },
    };
    AddInterface(*intf, ANNOUNCED);
    QStatus status = AddMethodHandlers(handlers, sizeof(handlers) / sizeof(handlers[0]));
    if (ER_OK != status) {
        fprintf(stderr, "Add handlers failed - %s", QCC_StatusText(status));
        return status;
    }

    status = m_bus->RegisterBusObject(*this);
    if (status != ER_OK) {
        fprintf(stderr, "Register bus object failed - %s\n", QCC_StatusText(status));
        return status;
    }

    SessionPort port = 0;
    SessionOpts opts;
    status = m_bus->BindSessionPort(port, opts, *static_cast<SessionPortListener*>(this));
    if (status != ER_OK) {
        fprintf(stderr, "BindSessionPort failed - %s\n", QCC_StatusText(status));
        return status;
    }

    m_aboutObj = new AboutObj(*m_bus);
    status = m_aboutObj->Announce(port, m_aboutData);
    if (status != ER_OK) {
        fprintf(stderr, "Announce failed - %s\n", QCC_StatusText(status));
        return status;
    }

    printf("Producer started\n");
    return status;
}

QStatus Producer::Add(const Entity& entity)
{
    if (find(m_matches.begin(), m_matches.end(), entity) == m_matches.end()) {
        m_matches.push_back(entity);
        MatchesChanged();
        return ER_OK;
    } else {
        return ER_FAIL;
    }
}

const Matches& Producer::GetMatches()
{
    return m_matches;
}

QStatus Producer::Remove(const Entity& entity)
{
    Matches::iterator it = find(m_matches.begin(), m_matches.end(), entity);
    if (it != m_matches.end()) {
        m_matches.erase(it);
        MatchesChanged();
        return ER_OK;
    } else {
        return ER_FAIL;
    }
}

void Producer::PresenceSubscribe(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_UNUSED(member);

    String path = "/" + U32ToString(m_presenceTrackers.size());
    PresenceTrackerObj* trackerObj = new PresenceTrackerObj(this, path.c_str());
    QStatus status = trackerObj->Register(m_bus);
    if (status == ER_OK) {
        m_presenceTrackers.push_back(trackerObj);
        MsgArg arg("o", trackerObj->GetPath());
        status = MethodReply(msg, &arg, 1);
    } else {
        delete trackerObj;
        status = MethodReply(msg, status);
    }
    if (status != ER_OK) {
        fprintf(stderr, "PresenceSubscribe reply failed - %s\n", QCC_StatusText(status));
    }
}

void Producer::MatchesChanged()
{
    const char* propNames[] = { "Matches" };
    for (vector<PresenceTrackerObj*>::iterator it = m_presenceTrackers.begin(); it != m_presenceTrackers.end(); ++it) {
        QStatus status = (*it)->EmitPropChanged(org::alljoyn::locationservices::PresenceTracker::InterfaceName,
                                                propNames, 1, SESSION_ID_ALL_HOSTED);
        if (status != ER_OK) {
            fprintf(stderr, "Emit properties changed failed - %s\n", QCC_StatusText(status));
        }
    }
}

PresenceTrackerObj::PresenceTrackerObj(Producer* producer, const char* path)
    : BusObject(path)
    , m_producer(producer)
    , m_version(1)
{
}

QStatus PresenceTrackerObj::Register(BusAttachment* bus)
{
    const InterfaceDescription* intf = bus->GetInterface(org::alljoyn::locationservices::PresenceTracker::InterfaceName);
    assert(intf);
    AddInterface(*intf);

    QStatus status = bus->RegisterBusObject(*this);
    if (status != ER_OK) {
        fprintf(stderr, "Register bus object failed - %s\n", QCC_StatusText(status));
    }
    return status;
}

QStatus PresenceTrackerObj::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    if (!strcmp(ifcName, org::alljoyn::locationservices::PresenceTracker::InterfaceName)) {
        if (!strcmp(propName, "Version")) {
            return val.Set("q", m_version);
        } else if (!strcmp(propName, "Matches")) {
            const Matches& matches = m_producer->GetMatches();
            MsgArg* matchesArgs = new MsgArg[matches.size()];
            size_t n = 0;
            for (Matches::const_iterator it = matches.begin(); it < matches.end(); ++it, ++n) {
                matchesArgs[n].Set("(say)", it->id.c_str(), 6, it->mac);
            }
            QStatus status = val.Set("a(say)", n, matchesArgs);
            if (status != ER_OK) {
                delete [] matchesArgs;
                return status;
            }
            val.SetOwnershipFlags(MsgArg::OwnsArgs, true);
            return status;
        }
    }
    return ER_BUS_NO_SUCH_PROPERTY;
}

int CDECL_CALL main(int, char* argv[])
{
    printf("AllJoyn Library version: %s\n", ajn::GetVersion());
    printf("AllJoyn Library build info: %s\n", ajn::GetBuildInfo());

#ifdef QCC_OS_LINUX
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = SigIntHandler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &action, NULL);
#else
    signal(SIGINT, SigIntHandler);
#endif

    if (AllJoynInit() != ER_OK) {
        return 1;
    }
#ifdef ROUTER
    if (AllJoynRouterInit() != ER_OK) {
        AllJoynShutdown();
        return 1;
    }
#endif

    QStatus status = ER_OK;
    BusAttachment* bus = NULL;
    Producer* producer = NULL;
    const int bufSize = 1024;
    char buf[bufSize];
    FILE* fp = stdin;

    bus = new BusAttachment(argv[0], true);
    status = CreateInterfaces(bus);
    if (status != ER_OK) {
        fprintf(stderr, "CreateInterfaces failed - %s\n", QCC_StatusText(status));
        goto exit;
    }
    status = bus->Start();
    if (status != ER_OK) {
        fprintf(stderr, "Start failed - %s\n", QCC_StatusText(status));
        goto exit;
    }
    status = bus->Connect();
    if (status != ER_OK) {
        fprintf(stderr, "Connect failed - %s\n", QCC_StatusText(status));
        goto exit;
    }

    for (;;) {
        if (g_interrupt) {
            break;
        }
        if (GetLine(buf, bufSize, fp) == NULL) {
            break;
        }
        String line(buf);
        String cmd = NextTok(line);
        if (cmd == "start") {
            producer = new Producer();
            status = producer->Start(bus);
        } else if (cmd == "add") {
            String id = NextTok(line);
            String mac = NextTok(line);
            status = producer->Add(Entity(id, reinterpret_cast<const uint8_t*>(mac.data())));
        } else if (cmd == "remove") {
            String id = NextTok(line);
            String mac = NextTok(line);
            status = producer->Remove(Entity(id, reinterpret_cast<const uint8_t*>(mac.data())));
        } else if (cmd == "exit") {
            break;
        }
        printf("%s\n", QCC_StatusText(status));
    }

exit:
    bus->Stop();
    bus->Join();
    delete producer;
    delete bus;
#ifdef ROUTER
    AllJoynRouterShutdown();
#endif
    AllJoynShutdown();
    return (int) status;
}
