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
/* ****NOTES - ADD LOCKING FOR READ READ/WRITE **** */
/* ****NOTES - ADD FILTERING ... CURRENTLY MATCHES ON ALL */

#include "LsInclude.h"


using namespace std;
using namespace qcc;

static qcc::String GetSignature(qcc::String uuid, qcc::String mac) 
{
    qcc::String signature = uuid + "//" + mac;
    return(signature);
}

static bool IsFilterMatch(qcc::String signature, DbFilter* filter) {
    bool result = false;
    
    if ((filter->uuidParser() == "*") && (filter->macParser() == "*")) {
        result = true;
        
    } else {
        std::vector<DbDevice>::iterator it;
        for (it = filter->list.begin(); !result && (it != filter->list.end()); it++) {
            qcc::String filterSignature = GetSignature(it->uuid, it->mac);
            result = (signature == filterSignature);
        }
    }
    return result;
}




class DiscoveryRecord : DbRecord
{
private:
    qcc::Mutex discoveryMutex;

public:
    DiscoveryRecord(qcc::String discoverySignature) 
        : DbRecord(discoverySignature), discoveryMutex() {
        SetDetectDiscovery(false, 0);
        SetRangeDiscovery((double)-1, 0);
        return;
    }

    qcc::String DiscoverySignature() {
        String result;
        discoveryMutex.Lock(MUTEX_CONTEXT);
        result = discoverySignature;
        discoveryMutex.Unlock();
        return result;
    }
    
    void SetDetectDiscovery(bool detectDiscovery, uint32_t detectTimestamp) {
        discoveryMutex.Lock(MUTEX_CONTEXT);
        this->detectDiscovery = detectDiscovery;
        this->detectTimestamp = detectTimestamp;
        discoveryMutex.Unlock();
    }
    
    void SetRangeDiscovery(double rangeDiscovery, uint32_t rangeTimestamp) {
        discoveryMutex.Lock(MUTEX_CONTEXT);
        this->detectDiscovery = true;
        this->detectTimestamp = rangeTimestamp;
        this->rangeDiscovery = rangeDiscovery;
        this->rangeTimestamp = rangeTimestamp;
        discoveryMutex.Unlock();
    }

    DbRecord* ResultRecord() {
        //QCC_DbgPrintf(("ResultRecord %s\n", discoverySignature.c_str()));

        discoveryMutex.Lock(MUTEX_CONTEXT);
        DbRecord* resultRecord = new DbRecord(discoverySignature);
        resultRecord->detectTimestamp = detectTimestamp;
        resultRecord->rangeTimestamp = rangeTimestamp;
        resultRecord->detectDiscovery = detectDiscovery;
        resultRecord->rangeDiscovery = rangeDiscovery;
        discoveryMutex.Unlock();
        return resultRecord;
    }
};


class DbTable 
{
private:
    qcc::String deviceSignature;
    qcc::Mutex recordsMutex;
    std::unordered_map<std::string, DiscoveryRecord*> discoveryRecords;

public:
    DbTable(qcc::String deviceSignature) 
        : deviceSignature(deviceSignature), recordsMutex(), discoveryRecords() {
        discoveryRecords.reserve(5);
    }

    qcc::String DeviceSignature() {
        return(deviceSignature);
    }

    DiscoveryRecord* GetDiscoveryRecord(qcc::String discoverySignature, bool createIfNoexist) {
        recordsMutex.Lock(MUTEX_CONTEXT);
        std::unordered_map<std::string, DiscoveryRecord*>::iterator it = discoveryRecords.find(discoverySignature.c_str());
        DiscoveryRecord* record = (it != discoveryRecords.end()) ? it->second : NULL;

        if ((record == NULL) && createIfNoexist) {
            record = new DiscoveryRecord(discoverySignature);
            discoveryRecords.emplace(discoverySignature.c_str(), record);
        }  
        recordsMutex.Unlock();
        return record;
    }

    void FilterRecords(DbFilter* recordMatcher, DbResult* dbResult) {
        recordsMutex.Lock(MUTEX_CONTEXT);
        std::unordered_map<std::string, DiscoveryRecord*>::iterator it;
        for (it=discoveryRecords.begin(); it != discoveryRecords.end(); it++) {
            DiscoveryRecord* discoveryRecord = it->second;
            if (IsFilterMatch(discoveryRecord->DiscoverySignature(), recordMatcher)) {
                DbRecord* resultRecord = discoveryRecord->ResultRecord();
                dbResult->records.push_back(resultRecord);
            }
        }
        recordsMutex.Unlock();
    }

};


LsDatabase::LsDatabase() : dbTablesMutex(), dbListenersMutex(), dbTables(), dbListeners()
{
    dbTables.reserve(25);
    dbListeners.reserve(25);
}

LsDatabase::~LsDatabase()
{
    QCC_DbgPrintf(("~LsDatabase"));

    dbTablesMutex.Lock(MUTEX_CONTEXT); 
    dbTables.clear();
    dbTablesMutex.Unlock();

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    dbListeners.erase(dbListeners.begin(), dbListeners.end());
    dbListenersMutex.Unlock();
}

qcc::String LsDatabase::DeviceSignature(qcc::String uuid, qcc::String mac) 
{
    qcc::String signature = GetSignature(uuid, mac);
    return(signature);
}


qcc::String LsDatabase::DeviceUuid(qcc::String signature)
{
    size_t pos = signature.find("//");
    qcc::String deviceUuid = (pos != qcc::String::npos)
        ? (signature.substr(0, pos) + "\0")
        : String("");

    //QCC_DbgPrintf(("DeviceUuid %s converted to %s\n", signature.c_str(), deviceUuid.c_str()));
    return deviceUuid;
}
    
qcc::String LsDatabase::DeviceMac(qcc::String signature)
{
    size_t pos = signature.find("//");
    qcc::String deviceMac = (pos != qcc::String::npos)
        ? (signature.substr((pos + 2), (signature.size() - (pos + 2))) + "\0")
        : String("");

    //QCC_DbgPrintf(("DeviceMac %s converted to %s\n", signature.c_str(), deviceMac.c_str()));
    return deviceMac;
}

QStatus LsDatabase::RegisterListener(DbListener* listener) {
    QCC_DbgPrintf(("RegisterListener"));

    QStatus status = ER_OK;

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Try to find an existing registration for the listener. */
        std::unordered_map<std::string, DbListener*>::iterator it = dbListeners.find(listener->identity.c_str());
        if (it == dbListeners.end()) {
            dbListeners.emplace(listener->identity.c_str(), listener);
        } else {
            status = ER_FAIL;
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("RegisterListener Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    dbListenersMutex.Unlock();
    return status;
}

QStatus LsDatabase::DeregisterListener(DbListener* listener) {
    QCC_DbgPrintf(("DeregisterListener"));

    QStatus status = ER_OK;

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Remove the listener registration. */
        std::unordered_map<std::string, DbListener*>::iterator it = dbListeners.find(listener->identity.c_str());
        if (it != dbListeners.end()) {
            dbListeners.erase(it);
        } else {
            status = ER_FAIL;
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("DeregisterListener Exception: %s\n", e.what()));
        status = ER_FAIL;
    }
    dbListenersMutex.Unlock();
    return status;
}

QStatus LsDatabase::PostDetectDiscovery(bool detectDiscovery,
                                        uint32_t discoveryTimestamp,
                                        const DbDevice& sensorDevice,
                                        const DbDevice& entityDevice) 
{
    QCC_DbgPrintf(("PostDetectDiscovery %s:%s\n", entityDevice.uuid.c_str(), entityDevice.mac.c_str()));

    QStatus status = ER_OK;

    try {
        DbTable *table;
        DiscoveryRecord *record;

        /* Bump the timestamp so it is never 0. */
        discoveryTimestamp++;

        /* Get the device signatures. */
        qcc::String sensorSignature = GetSignature(sensorDevice.uuid, sensorDevice.mac);
        qcc::String entitySignature = GetSignature(entityDevice.uuid, entityDevice.mac);

        /* Setup a discovery table/record for the sensor device. */
        table = GetTable(sensorSignature, true);
        record = table->GetDiscoveryRecord(entitySignature, true);
        record->SetDetectDiscovery(detectDiscovery, discoveryTimestamp);

        /* Setup a discovery table/record for the entity device. */
        table = GetTable(entitySignature, true);
        record = table->GetDiscoveryRecord(sensorSignature, true);
        record->SetDetectDiscovery(detectDiscovery, discoveryTimestamp);

        /* Send out notifications. */
        NotifyDetectDiscovery(sensorSignature, entitySignature, discoveryTimestamp, detectDiscovery);

    } catch(std::exception& e) {
        QCC_DbgPrintf(("PostDetectDiscovery Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    return status;
}

QStatus LsDatabase::PostRangeDiscovery(double rangeDiscovery,
                                       uint32_t discoveryTimestamp,
                                       const DbDevice& sensorDevice,
                                       const DbDevice& entityDevice) 
{
    QCC_DbgPrintf(("PostRangeDiscovery - %f", rangeDiscovery));

    QStatus status = ER_OK;

    try {
        DbTable *table;
        DiscoveryRecord *record;

        /* Bump the timestamp so it is never 0. */
        discoveryTimestamp++;

        /* Get the device signatures. */
        qcc::String sensorSignature = GetSignature(sensorDevice.uuid, sensorDevice.mac);
        qcc::String entitySignature = GetSignature(entityDevice.uuid, entityDevice.mac);

        /* Setup a discovery table/record for the sensor device. */
        table = GetTable(sensorSignature, true);
        record = table->GetDiscoveryRecord(entitySignature, true);
        record->SetRangeDiscovery(rangeDiscovery, discoveryTimestamp);

        /* Setup a discovery table/record for the entity device. */
        table = GetTable(entitySignature, true);
        record = table->GetDiscoveryRecord(sensorSignature, true);
        record->SetRangeDiscovery(rangeDiscovery, discoveryTimestamp);

        /* Send out notifications. */
        NotifyDetectDiscovery(sensorSignature, entitySignature, discoveryTimestamp, true);
        NotifyRangeDiscovery(sensorSignature, entitySignature, discoveryTimestamp, rangeDiscovery);
            
    } catch(std::exception& e) {
        QCC_DbgPrintf(("PostRangeDiscovery Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    return status;
}

QStatus LsDatabase::FilterDevices(DbDevice *tableMatcher, DbFilter* recordMatcher, DbResult* dbResult)
{
    QStatus status = ER_OK;
    DbTable *table;

    /* If a table matcher is specified, we limit the filtering to that table; otherwise, we filter
       the records across all the tables. */
    if (tableMatcher != NULL) {
        try {
            qcc::String tableSignature = GetSignature(tableMatcher->uuid, tableMatcher->mac);
            table = GetTable(tableSignature, true);
            table->FilterRecords(recordMatcher, dbResult);

        } catch(std::exception& e) {
            QCC_DbgPrintf(("FilterDevices Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }

    } else {
        dbTablesMutex.Lock(MUTEX_CONTEXT);
        try {
            std::unordered_map<std::string, DbTable*>::iterator it;
            for (it=dbTables.begin(); it != dbTables.end(); it++) {
                table = it->second;
                table->FilterRecords(recordMatcher, dbResult);
            }

        } catch(std::exception& e) {
            QCC_DbgPrintf(("FilterDevices Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
        dbTablesMutex.Unlock();
    }
    return status;
}

bool LsDatabase::FilterSignature(qcc::String signature, DbFilter* filter)
{
    bool result = IsFilterMatch(signature, filter);
    return result;
}

DbTable *LsDatabase::GetTable(qcc::String deviceSignature, bool createIfNoexist)
{
    DbTable* table = NULL;
    
    dbTablesMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Try to find an existing database table for the entity with the specified signature. */
        std::unordered_map<std::string, DbTable*>::iterator it = dbTables.find(deviceSignature.c_str());
        if (it != dbTables.end()) {
            table = it->second;
        }
        
        /* If no such table exists, create one if requested. */
        if ((table == NULL) && createIfNoexist) {
            table = new DbTable(deviceSignature);
            dbTables.emplace(deviceSignature.c_str(), table);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("GetTable Exception: %s\n", e.what())); 
    }
    dbTablesMutex.Unlock();
    return table;
}

void LsDatabase::NotifyDetectDiscovery(qcc::String sensorSignature,
                                       qcc::String entitySignature, 
                                       uint32_t timestamp, 
                                       bool detect) 
{
    QCC_DbgPrintf(("NotifyDetectDiscovery"));

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try{
        std::unordered_map<std::string, DbListener*>::iterator it;
        for (it=dbListeners.begin(); it != dbListeners.end(); it++) {
            it->second->DetectEvent(sensorSignature, entitySignature, timestamp, detect);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("NotifyDetectDiscovery Exception: %s\n", e.what())); 
    }
    dbListenersMutex.Unlock();
    return;
}

void LsDatabase::NotifyRangeDiscovery(qcc::String sensorSignature, 
                                      qcc::String entitySignature,
                                      uint32_t timestamp, 
                                      double range)
{
    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try{
        std::unordered_map<std::string, DbListener*>::iterator it;
        for (it=dbListeners.begin(); it != dbListeners.end(); it++) {
            it->second->RangeEvent(sensorSignature, entitySignature, timestamp, range);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("NotifyRangeDiscovery Exception: %s\n", e.what())); 
    }
    dbListenersMutex.Unlock();
    return;
}

