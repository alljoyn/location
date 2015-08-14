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

#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>
#include <alljoyn/location/LocationStd.h>
#include <alljoyn/version.h>

#include <qcc/String.h>
#include <qcc/StringUtil.h>

#include <signal.h>
#include <stdio.h>

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

class Consumer : public AboutListener, public ProxyBusObject::PropertiesChangedListener {
  public:
    virtual ~Consumer() { }
    QStatus Start(BusAttachment* bus);
  private:
    BusAttachment* m_bus;
    virtual void Announced(const char* name, uint16_t version, SessionPort port,
                           const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg);
    virtual void PropertiesChanged(ProxyBusObject& obj, const char* ifaceName,
                                   const MsgArg& changed, const MsgArg& invalidated,
                                   void* context);
};

QStatus Consumer::Start(BusAttachment* bus)
{
    m_bus = bus;
    m_bus->RegisterAboutListener(*this);
    QStatus status = m_bus->WhoImplements(org::alljoyn::locationservices::InterfaceName);
    if (status != ER_OK) {
        fprintf(stderr, "WhoImplements failed - %s\n", QCC_StatusText(status));
    }
    return status;
}

void Consumer::Announced(const char* name, uint16_t, SessionPort port,
                         const MsgArg& objectDescriptionArg, const MsgArg&)
{
    m_bus->EnableConcurrentCallbacks();
    SessionId sid;
    SessionOpts opts;
    QStatus status = m_bus->JoinSession(name, port, NULL, sid, opts);
    if (status != ER_OK) {
        fprintf(stderr, "JoinSession failed - %s\n", QCC_StatusText(status));
        return;
    }
    printf("Consumer joined %s sid=%d\n", name, sid);

    AboutObjectDescription objs(objectDescriptionArg);
    size_t numPaths = objs.GetInterfacePaths(org::alljoyn::locationservices::InterfaceName, NULL, 0);
    const char** paths = new const char*[numPaths];
    objs.GetInterfacePaths(org::alljoyn::locationservices::InterfaceName, paths, numPaths);
    for (size_t i = 0; i < numPaths; ++i) {
        printf("Location producer at %s %s %d\n", name, paths[i], sid);
    }
    delete [] paths;
}

void Consumer::PropertiesChanged(ProxyBusObject& obj, const char* ifaceName,
                                 const MsgArg& changed, const MsgArg& invalidated,
                                 void*)
{
    QCC_UNUSED(invalidated);

    printf("%s%s %s\n", obj.GetUniqueName().c_str(), obj.GetPath().c_str(), ifaceName);

    size_t numEntries;
    MsgArg* entries;
    changed.Get("a{sv}", &numEntries, &entries);
    for (size_t i = 0; i < numEntries; ++i) {
        char* key;
        MsgArg* value;
        entries[i].Get("{sv}", &key, &value);
        if (!strcmp(key, "Matches")) {
            size_t numEntities;
            MsgArg* entities;
            value->Get("a(say)", &numEntities, &entities);
            for (size_t j = 0; j < numEntities; ++j) {
                char* id;
                size_t n;
                uint8_t* mac;
                entities[j].Get("(say)", &id, &n, &mac);
                printf("  %s ", id);
                for (size_t k = 0; k < n; ++k) {
                    printf("%c", mac[k]);
                }
                printf("\n");
            }
        }
    }
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
    Consumer* consumer = NULL;
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
            consumer = new Consumer();
            status = consumer->Start(bus);

        } else if (cmd == "subscribe") {
            String name = NextTok(line);
            String path = NextTok(line);
            SessionId sid = static_cast<SessionId>(StringToU32(NextTok(line), 0, 0));

            ProxyBusObject proxyObj(*bus, name.c_str(), path.c_str(), sid);
            status = proxyObj.AddInterface(org::alljoyn::locationservices::InterfaceName);
            assert(status == ER_OK);
            MsgArg arg("(ss)", "idFilter", "macFilter");
            Message reply(*bus);
            status = proxyObj.MethodCall(org::alljoyn::locationservices::InterfaceName, "PresenceSubscribe",
                                         &arg, 1, reply);
            if (status == ER_OK) {
                char* trackerPath;
                status = reply->GetArgs("o", &trackerPath);
                assert(status == ER_OK);
                ProxyBusObject* trackerProxyObj = new ProxyBusObject(*bus, name.c_str(), trackerPath, sid);
                status = trackerProxyObj->RegisterPropertiesChangedListener(org::alljoyn::locationservices::PresenceTracker::InterfaceName,
                                                                            NULL, 0, *consumer, NULL);
            }
        } else if (cmd == "exit") {
            break;
        }
        printf("%s\n", QCC_StatusText(status));
    }

exit:
    bus->Stop();
    bus->Join();
    delete consumer;
    delete bus;
#ifdef ROUTER
    AllJoynRouterShutdown();
#endif
    AllJoynShutdown();
    return (int) status;
}
