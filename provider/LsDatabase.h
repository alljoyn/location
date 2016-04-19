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
#ifndef LS_DATABASE_H_
#define LS_DATABASE_H_


class DbTable;

class DbDevice
{
public:
    qcc::String uuid;
    qcc::String mac;
    DbDevice(qcc::String uuid, qcc::String mac) : uuid(uuid), mac(mac) {return;}
};

class DbFilter : DbDevice
{
public:
    std::vector<DbDevice> list;
    qcc::String uuidParser() {return uuid;}
    qcc::String macParser() {return mac;}
    DbFilter(qcc::String uuid, qcc::String mac) : DbDevice(uuid, mac), list() {return;}
};

class DbRecord 
{
public:
    qcc::String discoverySignature;

    uint32_t detectTimestamp;
    uint32_t rangeTimestamp;
    
    bool detectDiscovery;
    double rangeDiscovery;

    DbRecord(qcc::String discoverySignature) 
        : discoverySignature(discoverySignature), detectTimestamp(0), rangeTimestamp(0) {return;}
};

class DbResult
{
public:
    std::vector<DbRecord*> records;

    DbResult() : records() {
        return;
    }
    ~DbResult()
    {
        std::vector<DbRecord *>::iterator it;
        for (it = records.begin(); it != records.end(); it++) {
            delete *it;
        }
    }       
};
                
class DbListener 
{
public:
    qcc::String identity;

    DbListener(qcc::String identity) : identity(identity) {;}
    virtual void DetectEvent(qcc::String sensorSignature, 
                             qcc::String entitySignature,
                             uint32_t timestamp, 
                             bool detect) {;};
    virtual void RangeEvent(qcc::String sensorSignature,
                            qcc::String entitySignature,
                            uint32_t timestamp, 
                            double range) {;};
};

class LsDatabase
{
private:
    qcc::Mutex dbTablesMutex;
    qcc::Mutex dbListenersMutex;

    std::unordered_map<std::string, DbTable*> dbTables;
    std::unordered_map<std::string, DbListener*> dbListeners;


    DbTable *GetTable(qcc::String deviceSignature, bool createIfNoexist);
    void NotifyDetectDiscovery(qcc::String sensorSignature, 
                               qcc::String entitySignature, 
                               uint32_t timestamp, 
                               bool detect);
    void NotifyRangeDiscovery(qcc::String sensorSignature, 
                              qcc::String entitySignature, 
                              uint32_t timestamp, 
                              double range);
    
public:
    LsDatabase();
    ~LsDatabase();

    qcc::String DeviceSignature(qcc::String uuid, qcc::String mac);
    qcc::String DeviceUuid(qcc::String signature);
    qcc::String DeviceMac(qcc::String signature);

    QStatus RegisterListener(DbListener* listener);
    QStatus DeregisterListener(DbListener* listener);
 
    QStatus PostDetectDiscovery(bool detectDiscovery,
                                uint32_t discoveryTimestamp,
                                const DbDevice& sensorDevice,
                                const DbDevice& entityDevice);
    QStatus PostRangeDiscovery(double rangeDiscovery,
                               uint32_t discoveryTimestamp,
                               const DbDevice& sensorDevice,
                               const DbDevice& entityDevice);

    QStatus FilterDevices(DbDevice *tableMatcher, DbFilter* recordMatcher, DbResult* dbResult);
    bool FilterSignature(qcc::String signature, DbFilter* filter); 

};            


#endif /* LS_DATABASE_H_ */
