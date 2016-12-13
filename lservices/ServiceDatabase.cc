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


using namespace std;
using namespace qcc;

static const qcc::String defaultPhrase = qcc::String(""); 
static DbItem* defaultPhraseItem = new DbItem(defaultPhrase);

static pthread_mutex_t dbCurrentCursorMutex;
static DbCursor dbCurrentCursor = 0;
static DbCursor DbCurrentCursor() 
{
    //dbCurrentCursorMutex.Lock(MUTEX_CONTEXT);
    pthread_mutex_lock(&dbCurrentCursorMutex);
    DbCursor result = ++dbCurrentCursor;
    //dbCurrentCursorMutex.Unlock();
    pthread_mutex_unlock(&dbCurrentCursorMutex);
    return result;
}
static std::string MakeRowKey(DbCursor cursor1, DbCursor cursor2) 
{
    std::string key = (cursor1 < cursor2)
        ? std::to_string(cursor1) + "::::" + std::to_string(cursor2)
        : std::to_string(cursor2) + "::::" + std::to_string(cursor1);
    return key;
}



DbIdentity::DbIdentity(const qcc::String& signature)
    : signature(new qcc::String(signature))
{
    cursor = DbCurrentCursor();
}
DbIdentity::~DbIdentity()
{
    QCC_DbgPrintf(("~DbIdentity"));
    delete signature;
}
DbCursor DbIdentity::GetCursor() 
{
    return cursor;
}
const qcc::String& DbIdentity::GetSignature()
{
    return *signature;
}



DbItem::DbItem(const qcc::String& signature)
    : DbIdentity(signature), aliasMutex(), aliasList()
{
    return;
}
DbItem::~DbItem()
{
    QCC_DbgPrintf(("~DbItem"));
    aliasList.clear();
    return;
}
bool DbItem::PutAlias(DbIdentity* alias)
{
    bool result = false;

    if (alias != NULL) {
        int position = -1;

        aliasMutex.Lock(MUTEX_CONTEXT);
        for (int index = 0; (position < 0) && (index < (int)aliasList.size()); index++) {
            if (alias == aliasList.at(index)) {
                position = index;
            }
        }
        if (position < 0) {
            result = true;
            aliasList.push_back(alias);
        }
        aliasMutex.Unlock();
    }
    return result;
}
DbIdentity* DbItem::GetAlias(int position) 
{
    DbIdentity* result;

    aliasMutex.Lock(MUTEX_CONTEXT);
    result = ((position >= 0) && (position < (int)aliasList.size())) ? aliasList.at(position) : NULL;
    aliasMutex.Unlock();

    return result;
}
DbIdentity* DbItem::GetAlias(qcc::String& signature) 
{
    DbIdentity* result = NULL;

    DbIdentity* alias;
    for (int pos = 0; (result == NULL) && ((alias = GetAlias(pos)) != NULL); pos++) {
        if (signature == alias->GetSignature()) {
            result = alias;
        }
    }
    return result;
}



DbEntity::DbEntity(DbCursor deviceCursor, DbCursor phraseCursor)
    : deviceCursor(deviceCursor), phraseCursor(phraseCursor)
{
    return;
}



DbParser::DbParser(const qcc::String& uidExp, const qcc::String& descriptorExp)
    : deviceExpression(), phraseExpression()
{
    //QCC_DbgPrintf(("DbParser %s : %s", uidExp.c_str(), descriptorExp.c_str()));
    try {
        deviceExpression = std::regex(uidExp.c_str(), std::regex::grep);
        phraseExpression = std::regex(descriptorExp.c_str(), std::regex::grep);

    } catch(std::exception& e) {
        QCC_DbgPrintf(("DbEntity Exception: %s\n", e.what())); 
    }
}



DbFilter::DbFilter(DbCursor refDevice, DbCursor refPhrase)
    : refDevice(refDevice), refPhrase(refPhrase), parsersMutex(), parsers()
{
    return;
}
DbFilter::~DbFilter()
{
    QCC_DbgPrintf(("~DbFilter"));
    parsers.clear();
}
DbCursor DbFilter::GetRefDevice()
{
    return refDevice;
}
DbCursor DbFilter::GetRefPhrase() 
{
    return refPhrase;
}
void DbFilter::AddParser(const qcc::String& uidExp, const qcc::String& descriptorExp)
{
    parsersMutex.Lock(MUTEX_CONTEXT);
    parsers.push_back(DbParser(uidExp, descriptorExp));
    parsersMutex.Unlock();
}
bool DbFilter::IsMatch(const qcc::String& uid)
{
    QCC_DbgPrintf(("IsMatch %s", uid.c_str()));

    bool result = false;

    parsersMutex.Lock(MUTEX_CONTEXT);
    std::vector<DbParser>::iterator it;
    for (it = parsers.begin(); !result && (it != parsers.end()); it++) {
        result = (std::regex_match(uid.c_str(), it->deviceExpression));              
    }
    parsersMutex.Unlock();
    return result;
}
bool DbFilter::IsMatch(const qcc::String& uid, const qcc::String& descriptor)
{
    QCC_DbgPrintf(("IsMatch %s : %s", uid.c_str(), descriptor.c_str()));

    bool result = false;

    parsersMutex.Lock(MUTEX_CONTEXT);
    std::vector<DbParser>::iterator it;
    for (it = parsers.begin(); !result && (it != parsers.end()); it++) {
        result = ((std::regex_match(uid.c_str(), it->deviceExpression)) && 
                  (std::regex_match(descriptor.c_str(), it->phraseExpression)));              
    }
    parsersMutex.Unlock();
    return result;
}
bool DbFilter::IsMatch(DbIdentity *device, DbIdentity* phrase) 
{
    bool result = false;
    if ((device!= NULL) && (phrase != NULL)) {
        parsersMutex.Lock(MUTEX_CONTEXT);
        std::vector<DbParser>::iterator it;
        for (it = parsers.begin(); !result && (it != parsers.end()); it++) {
            result = ((std::regex_match(device->GetSignature().c_str(), it->deviceExpression)) && 
                      (std::regex_match(phrase->GetSignature().c_str(), it->phraseExpression)));              
        }
        parsersMutex.Unlock();
    }
    return result;
}



DbCache::DbCache(DbCursor refDevice, DbCursor refPhrase)
    : DbFilter(refDevice, refPhrase), entities()
{
    return;
}
DbCache::~DbCache()
{
    QCC_DbgPrintf(("~DbCache"));
    entities.clear();
}



DbRow::DbRow(DbCursor contributorCursor, DbCursor discoveryCursor)
    : timestamp(0), contributorCursor(contributorCursor), discoveryCursor(discoveryCursor)
{
    return;
}
DbRow::~DbRow()
{
    QCC_DbgPrintf(("~DbRow"));
}
DbCursor DbRow::GetContributorCursor() 
{
    return contributorCursor;
}
DbCursor DbRow::GetDiscoveryCursor() 
{
    return discoveryCursor;
}
DbCursor DbRow::GetLinkedCursor(DbCursor cursor)
{
    DbCursor result;
    if (cursor == contributorCursor) {
        result = discoveryCursor;

    } else if (cursor == discoveryCursor) {
        result = contributorCursor;

    } else {
        result = 0;
    }
    return result;
}



DbPresenceRow::DbPresenceRow(DbCursor contributorCursor, DbCursor discoveryCursor)
    : DbRow(contributorCursor, discoveryCursor),
      presenceMutex(), isPresent(false) 
{
    QCC_DbgPrintf(("DbPresenceRow %d:%d", contributorCursor, discoveryCursor));
    return;
}
DbPresenceRow::~DbPresenceRow()
{
    QCC_DbgPrintf(("~DbPresenceRow"));
}
bool DbPresenceRow::SetPresence(uint32_t timestamp, bool isPresent) 
{
    presenceMutex.Lock(MUTEX_CONTEXT);
    bool result = (this->timestamp == 0) || (this->isPresent != isPresent);
    this->timestamp = timestamp;
    this->isPresent = isPresent;
    presenceMutex.Unlock();

    return result;
}
bool DbPresenceRow::GetPresence()
{
    presenceMutex.Lock(MUTEX_CONTEXT);
    bool result = isPresent;
    presenceMutex.Unlock();

    return result;
}



DbDistanceRow::DbDistanceRow(DbCursor contributorCursor, DbCursor discoveryCursor)
    : DbRow(contributorCursor, discoveryCursor), 
      distanceMutex(), distance(MAX_DISTANCE), accuracy(0)
{
    QCC_DbgPrintf(("DbDistanceRow %d:%d", contributorCursor, discoveryCursor));
    return;
}
DbDistanceRow::~DbDistanceRow() 
{
    QCC_DbgPrintf(("~DbDistanceRow"));
}
bool DbDistanceRow::SetDistance(uint32_t timestamp, double distance, double accuracy)
{ 
    distanceMutex.Lock(MUTEX_CONTEXT);
    bool result = (this->timestamp == 0) || (this->distance != distance);
    this->timestamp = timestamp;
    this->distance = distance;
    this->accuracy = accuracy;
    distanceMutex.Unlock();

    return result;
}
double DbDistanceRow::GetDistance()
{
    distanceMutex.Lock(MUTEX_CONTEXT);
    double result = distance;
    distanceMutex.Unlock();

    return result;
}
double DbDistanceRow::GetAccuracy()
{
    distanceMutex.Lock(MUTEX_CONTEXT);
    double result = accuracy;
    distanceMutex.Unlock();

    return result;
}



DbPositionRow::DbPositionRow(DbCursor contributorCursor, DbCursor discoveryCursor)
    : DbRow(contributorCursor, discoveryCursor),
      positionMutex()
{
    return;
}
DbPositionRow::~DbPositionRow() 
{
    QCC_DbgPrintf(("~DbPositionRow"));
}
bool DbPositionRow::SetPosition(uint32_t timestamp,
                                Position position,
                                double accuracy)
{
    positionMutex.Lock(MUTEX_CONTEXT);
    bool result = (this->timestamp == 0);
    this->timestamp = timestamp;
    this->position = position;
    this->accuracy = accuracy;    
    positionMutex.Unlock();

    return result;
}
Position DbPositionRow::GetPosition()
{
    positionMutex.Lock(MUTEX_CONTEXT);
    Position result = position;
    positionMutex.Unlock();

    return result;
}
double DbPositionRow::GetAccuracy()
{
    positionMutex.Lock(MUTEX_CONTEXT);
    double result = accuracy;
    positionMutex.Unlock();

    return result;
}


DbTable::DbTable() 
    : columnsMutex(), columns()
{
    QCC_DbgPrintf(("DbTable"));
    return;
}
DbTable::~DbTable()
{
    columnsMutex.Lock(MUTEX_CONTEXT);
    columns.clear();
    columnsMutex.Unlock();
}
void DbTable::PutColumn(DbCursor cursor)
{
    QCC_DbgPrintf(("PutColumn - %d", cursor));

    int position = -1;

    columnsMutex.Lock(MUTEX_CONTEXT);
    for (int index = 0; (position < 0) && (index < (int)columns.size()); index++) {
        if (cursor == columns.at(index)) {
            position = index;
        }
    }
    if (position < 0) {
        columns.push_back(cursor);
    }
    columnsMutex.Unlock();
}
DbCursor DbTable::GetColumn(int position) 
{
    DbCursor result;

    columnsMutex.Lock(MUTEX_CONTEXT);
    result = ((position >= 0) && (position < (int)columns.size())) ? columns.at(position) : 0;
    columnsMutex.Unlock();

    return result;
}



DbListener::DbListener()
{
    cursor = DbCurrentCursor();
}
DbListener::~DbListener()
{
    QCC_DbgPrintf(("~DbListener"));
    return;
}
DbCursor DbListener::GetCursor()
{
    return cursor;
}
void DbListener::PresenceEvent(bool cacheFlush,
                               DbCursor contributorCursor,
                               DbCursor discoveryCursor,
                               bool presence)
{
    QCC_DbgPrintf(("PresenceEvent (default)"));
    return;
};
void DbListener::DistanceEvent(bool cacheFlush,
                               DbCursor contributorCursor,
                               DbCursor discoveryCursor,
                               double distance)
{
    QCC_DbgPrintf(("DistanceEvent (default)"));
    return;
}
void DbListener::PositionEvent(bool cacheFlush,
                               DbCursor contributorCursor,
                               DbCursor discoveryCursor,
                               Position position)
{
    QCC_DbgPrintf(("PositionEvent (default)"));
    return;
}



void ServiceDatabase::InjectPresenceColumn(DbCursor tableCursor, DbCursor columnCursor) 
{
    /* Make sure we know about the table. */
    DbTable* dbTable = NULL;
    dbPresenceTablesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<DbCursor, DbTable*>::iterator it = dbPresenceTables.find(tableCursor);
        if (it != dbPresenceTables.end()) {
            dbTable = it->second;
        } else {
            dbTable = new DbTable();
            dbPresenceTables.emplace(tableCursor, dbTable);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("InjectPresenceColumn Exception: %s\n", e.what())); 
    }
    dbPresenceTablesMutex.Unlock();

    /* Make sure the table has the specified column. */
    if (dbTable != NULL) {
        dbTable->PutColumn(columnCursor);
    }
}
void ServiceDatabase::InjectDistanceColumn(DbCursor tableCursor, DbCursor columnCursor) 
{
    QCC_DbgPrintf(("InjectDistanceColumn %d:%d", tableCursor, columnCursor));

    /* Make sure we know about the table. */
    DbTable* dbTable = NULL;
    dbDistanceTablesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<DbCursor, DbTable*>::iterator it = dbDistanceTables.find(tableCursor);
        if (it != dbDistanceTables.end()) {
            dbTable = it->second;
        } else {
            dbTable = new DbTable();
            dbDistanceTables.emplace(tableCursor, dbTable);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("InjectDistanceColumn Exception: %s\n", e.what())); 
    }
    dbDistanceTablesMutex.Unlock();

    /* Make sure the table has the specified column. */
    if (dbTable != NULL) {
        dbTable->PutColumn(columnCursor);
    }
}
void ServiceDatabase::InjectPositionColumn(DbCursor tableCursor, DbCursor columnCursor) 
{
    /* Make sure we know about the table. */
    DbTable* dbTable = NULL;
    dbPositionTablesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<DbCursor, DbTable*>::iterator it = dbPositionTables.find(tableCursor);
        if (it != dbPositionTables.end()) {
            dbTable = it->second;
        } else {
            dbTable = new DbTable();
            dbPositionTables.emplace(tableCursor, dbTable);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("InjectPositionColumn Exception: %s\n", e.what())); 
    }
    dbPositionTablesMutex.Unlock();

    /* Make sure the table has the specified column. */
    if (dbTable != NULL) {
        dbTable->PutColumn(columnCursor);
    }
}
void ServiceDatabase::NotifyPresence(bool cacheFlush, 
                                     DbCursor contributorCursor, 
                                     DbCursor discoveryCursor,
                                     bool presence)
{
    QCC_DbgPrintf(("NotifyPresence"));

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try{
        std::unordered_map<DbCursor, DbListener*>::iterator it;
        for (it=dbListeners.begin(); it != dbListeners.end(); it++) {
            it->second->PresenceEvent(cacheFlush, 
                                      contributorCursor, 
                                      discoveryCursor,
                                      presence);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("NotifyPresence Exception: %s\n", e.what())); 
    }
    dbListenersMutex.Unlock();
    return;
}
void ServiceDatabase::NotifyDistance(bool cacheFlush, 
                                     DbCursor contributorCursor, 
                                     DbCursor discoveryCursor,
                                     double distance)
{
    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try{
        std::unordered_map<DbCursor, DbListener*>::iterator it;
        for (it=dbListeners.begin(); it != dbListeners.end(); it++) {
            //it->second->PresenceEvent(cacheFlush, 
            //                          contributorCursor, 
            //                          discoveryCursor,
            //                          true);
            it->second->DistanceEvent(cacheFlush, 
                                      contributorCursor, 
                                      discoveryCursor,
                                      distance);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("NotifyDistance Exception: %s\n", e.what())); 
    }
    dbListenersMutex.Unlock();
    return;
}
void ServiceDatabase::NotifyPosition(bool cacheFlush, 
                                     DbCursor contributorCursor, 
                                     DbCursor discoveryCursor,
                                     Position position)
{
    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try{
        std::unordered_map<DbCursor, DbListener*>::iterator it;
        for (it=dbListeners.begin(); it != dbListeners.end(); it++) {
            //it->second->PresenceEvent(cacheFlush, 
            //                          contributorCursor, 
            //                          discoveryCursor,
            //                          true);
            it->second->PositionEvent(cacheFlush, 
                                      contributorCursor, 
                                      discoveryCursor,
                                      position);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("NotifyPosition Exception: %s\n", e.what())); 
    }
    dbListenersMutex.Unlock();
    return;
}
DbItem* ServiceDatabase::FindItem(DbCursor cursor)
{
    DbItem* result;

    dbItemsMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<DbCursor, DbItem&>::iterator it = dbItems.find(cursor);
        result = (it != dbItems.end()) ? &it->second : NULL;

    } catch(std::exception& e) {
        result = NULL;
        QCC_DbgPrintf(("FindItem Exception: %s\n", e.what())); 
    }
    dbItemsMutex.Unlock();
    
    return result;
}
void ServiceDatabase::CacheItem(DbCursor cursor, DbCache& cache) 
{
    QCC_DbgPrintf(("CacheItem"));

    DbItem* item = FindItem(cursor);
    if (item != NULL) {
        DbIdentity* alias;
        for (int pos = 0; ((alias = item->GetAlias(pos)) != NULL); pos++) {
            if (cache.IsMatch(item, alias)) {
                cache.entities.push_back(DbEntity(item->GetCursor(), alias->GetCursor()));
            }               
        }
    }
}
ServiceDatabase::ServiceDatabase() 
    : dbDevicesMutex(), dbDevices(), 
      dbPhrasesMutex(), dbPhrases(),
      dbItemsMutex(), dbItems(),
      dbListenersMutex(), dbListeners(), 
      dbPresenceRowsMutex(), dbPresenceRows(),
      dbDistanceRowsMutex(), dbDistanceRows(),
      dbPositionRowsMutex(), dbPositionRows(),
      dbPresenceTablesMutex(), dbPresenceTables(),
      dbDistanceTablesMutex(), dbDistanceTables(),
      dbPositionTablesMutex(), dbPositionTables()
{
    pthread_mutex_init(&dbCurrentCursorMutex, NULL);

    /* Reserve some space. */
    dbDevices.reserve(25);
    dbPhrases.reserve(25);
    dbItems.reserve(50);
    dbListeners.reserve(15);
    dbPresenceRows.reserve(15);
    dbDistanceRows.reserve(15);
    dbPositionRows.reserve(15);
    dbPresenceTables.reserve(20);
    dbDistanceTables.reserve(20);
    dbPositionTables.reserve(20);

    /* Always allow for the default phrase. */
    dbPhrasesMutex.Lock(MUTEX_CONTEXT);
    dbPhrases.emplace(defaultPhrase.c_str(), defaultPhraseItem);
    dbPhrasesMutex.Unlock();    

    dbItemsMutex.Lock(MUTEX_CONTEXT);
    dbItems.emplace(defaultPhraseItem->GetCursor(), *defaultPhraseItem);
    dbItemsMutex.Unlock();
}
ServiceDatabase::~ServiceDatabase()
{
    QCC_DbgPrintf(("~ServiceDatabase"));

    dbDevicesMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<std::string, DbItem*>::iterator dbDevicesIt;
    for (dbDevicesIt=dbDevices.begin(); dbDevicesIt != dbDevices.end(); dbDevicesIt++) {
        delete dbDevicesIt->second;
        dbDevicesIt->second = NULL;
    }        
    dbDevices.clear();
    dbDevicesMutex.Unlock();

    dbPhrasesMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<std::string, DbItem*>::iterator dbPhrasesIt;
    for (dbPhrasesIt=dbPhrases.begin(); dbPhrasesIt != dbPhrases.end(); dbPhrasesIt++) {
        delete dbPhrasesIt->second;
        dbPhrasesIt->second = NULL;
    }        
    dbPhrases.clear();
    dbPhrasesMutex.Unlock();

    dbItemsMutex.Lock(MUTEX_CONTEXT);
    dbItems.clear();
    dbItemsMutex.Unlock();

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    dbListeners.clear();
    dbListenersMutex.Unlock();

    dbPresenceRowsMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<std::string, DbPresenceRow*>::iterator dbPresenceRowsIt;
    for (dbPresenceRowsIt=dbPresenceRows.begin(); dbPresenceRowsIt != dbPresenceRows.end(); dbPresenceRowsIt++) {
        delete dbPresenceRowsIt->second;
        dbPresenceRowsIt->second = NULL;
    }        
    dbPresenceRows.clear();
    dbPresenceRowsMutex.Unlock();

    dbDistanceRowsMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<std::string, DbDistanceRow*>::iterator dbDistanceRowsIt;
    for (dbDistanceRowsIt=dbDistanceRows.begin(); dbDistanceRowsIt != dbDistanceRows.end(); dbDistanceRowsIt++) {
        delete dbDistanceRowsIt->second;
        dbDistanceRowsIt->second = NULL;
    }        
    dbDistanceRows.clear();
    dbDistanceRowsMutex.Unlock();

    dbPositionRowsMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<std::string, DbPositionRow*>::iterator dbPositionRowsIt;
    for (dbPositionRowsIt=dbPositionRows.begin(); dbPositionRowsIt != dbPositionRows.end(); dbPositionRowsIt++) {
        delete dbPositionRowsIt->second;
        dbPositionRowsIt->second = NULL;
    }        
    dbPositionRows.clear();
    dbPositionRowsMutex.Unlock();

    dbPresenceTablesMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<DbCursor, DbTable*>::iterator dbPresenceTablesIt;
    for (dbPresenceTablesIt=dbPresenceTables.begin(); dbPresenceTablesIt != dbPresenceTables.end(); dbPresenceTablesIt++) {
        delete dbPresenceTablesIt->second;
        dbPresenceTablesIt->second = NULL;
    }        
    dbPresenceTables.clear();
    dbPresenceTablesMutex.Unlock();

    dbDistanceTablesMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<DbCursor, DbTable*>::iterator dbDistanceTablesIt;
    for (dbDistanceTablesIt=dbDistanceTables.begin(); dbDistanceTablesIt != dbDistanceTables.end(); dbDistanceTablesIt++) {
        delete dbDistanceTablesIt->second;
        dbDistanceTablesIt->second = NULL;
    }        
    dbDistanceTables.clear();
    dbDistanceTablesMutex.Unlock();

    dbPositionTablesMutex.Lock(MUTEX_CONTEXT);
    std::unordered_map<DbCursor, DbTable*>::iterator dbPositionTablesIt;
    for (dbPositionTablesIt=dbPositionTables.begin(); dbPositionTablesIt != dbPositionTables.end(); dbPositionTablesIt++) {
        delete dbPositionTablesIt->second;
        dbPositionTablesIt->second = NULL;
    }        
    dbPositionTables.clear();
    dbPositionTablesMutex.Unlock();
}
DbCursor ServiceDatabase::MapDevice(const qcc::String& uid, const qcc::String& descriptor)
{
    DbCursor deviceCursor = 0;

    /* Make sure we know about the device. */
    DbItem* deviceItem = NULL;
    dbDevicesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<std::string, DbItem*>::iterator it = dbDevices.find(uid.c_str());
        if (it != dbDevices.end()) {
            deviceItem = it->second;
        } else {
            deviceItem = new DbItem(uid);
            dbDevices.emplace(uid.c_str(), deviceItem);

            dbItemsMutex.Lock(MUTEX_CONTEXT);
            dbItems.emplace(deviceItem->GetCursor(), *deviceItem);
            dbItemsMutex.Unlock();

            deviceItem->PutAlias(defaultPhraseItem);
            defaultPhraseItem->PutAlias(deviceItem);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("MapDevice Device Exception: %s\n", e.what())); 
    }
    dbDevicesMutex.Unlock();

    /* Make sure we know about the phrase. */
    DbItem* phraseItem = NULL;
    dbPhrasesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<std::string, DbItem*>::iterator it = dbPhrases.find(descriptor.c_str());
        if (it != dbPhrases.end()) {
            phraseItem = it->second;
        } else {
            phraseItem = new DbItem(descriptor);
            dbPhrases.emplace(descriptor.c_str(), phraseItem);

            dbItemsMutex.Lock(MUTEX_CONTEXT);
            dbItems.emplace(phraseItem->GetCursor(), *phraseItem);
            dbItemsMutex.Unlock();
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("MapDevice Phrase Exception: %s\n", e.what())); 
    }
    dbPhrasesMutex.Unlock();

    /* Link the device/phrase items. */
    if ((deviceItem != NULL) && (phraseItem != NULL)) {
        deviceItem->PutAlias(phraseItem);
        phraseItem->PutAlias(deviceItem);
        
        deviceCursor = deviceItem->GetCursor();
    } 
    return deviceCursor;
}
DbCursor ServiceDatabase::FindDevice(const qcc::String& uid, const qcc::String& descriptor)
{
    QCC_DbgPrintf(("FindDevice %s %s", uid.c_str(), descriptor.c_str()));
    DbItem* deviceItem = NULL;

    /* Make sure we have the device. */
    dbDevicesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<std::string, DbItem*>::iterator it = dbDevices.find(uid.c_str());
        if (it != dbDevices.end()) {
            deviceItem = it->second;
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("FindDevice Exception: %s\n", e.what())); 
    }
    dbDevicesMutex.Unlock();

    /* Make sure the device has the specified alias. */
    if (deviceItem != NULL) {
        qcc::String phraseAlias = descriptor.c_str();
        if (deviceItem->GetAlias(phraseAlias) == NULL) {
            deviceItem = NULL;
        }
    }

    DbCursor deviceCursor = (deviceItem != NULL) ? deviceItem->GetCursor() : 0;

    QCC_DbgPrintf(("FindDevice %d", deviceCursor));
    return deviceCursor;
}
DbCursor ServiceDatabase::FindPhrase(const qcc::String& descriptor)
{
    DbItem* phraseItem = NULL;
    dbPhrasesMutex.Lock(MUTEX_CONTEXT);
    try {
        std::unordered_map<std::string, DbItem*>::iterator it = dbPhrases.find(descriptor.c_str());
        if (it != dbPhrases.end()) {
            phraseItem = it->second;
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("FindPhrase Exception: %s\n", e.what())); 
    }
    dbPhrasesMutex.Unlock();

    DbCursor phraseCursor = (phraseItem != NULL) ? phraseItem->GetCursor() : 0;
    return phraseCursor;
}
QStatus ServiceDatabase::RegisterListener(DbListener* listener) 
{
    QCC_DbgPrintf(("RegisterListener"));

    QStatus status = ER_OK;

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Try to find an existing registration for the listener. */
        std::unordered_map<DbCursor, DbListener*>::iterator it = dbListeners.find(listener->GetCursor());
        if (it == dbListeners.end()) {
            dbListeners.emplace(listener->GetCursor(), listener);
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
QStatus ServiceDatabase::DeregisterListener(DbListener* listener) 
{
    QCC_DbgPrintf(("DeregisterListener"));

    QStatus status = ER_OK;

    dbListenersMutex.Lock(MUTEX_CONTEXT);
    try {
        /* Remove the listener registration. */
        std::unordered_map<DbCursor, DbListener*>::iterator it = dbListeners.find(listener->GetCursor());
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
QStatus ServiceDatabase::InjectPresence(bool cacheFlush,
                                        DbCursor contributorCursor,
                                        DbCursor discoveryCursor,
                                        bool presence)
{
    QCC_DbgPrintf(("InjectPresence"));
    
    QStatus status = ER_OK;
    try {            
        /* Get the timestamp to use. */
        uint32_t timestamp = ajn::BusAttachment::GetTimestamp();
        
        /* Make the row key. */
        std::string key = MakeRowKey(contributorCursor, discoveryCursor);
        
        /* Get the row to use, creating one if necessary. */
        DbPresenceRow* row;
        
        dbPresenceRowsMutex.Lock(MUTEX_CONTEXT);
        std::unordered_map<std::string, DbPresenceRow*>::iterator it = dbPresenceRows.find(key);
        if (it != dbPresenceRows.end()) {
            row = it->second;
        } else {
            row = new DbPresenceRow(contributorCursor, discoveryCursor);               
            dbPresenceRows.emplace(key, row);
        }
        dbPresenceRowsMutex.Unlock();
        
        /* Inject the presence information.  If the row is change, update the columns.*/
        bool rowChanged = row->SetPresence(timestamp, presence);
        if (rowChanged) {
            InjectPresenceColumn(contributorCursor, discoveryCursor);
            InjectPresenceColumn(discoveryCursor, contributorCursor);
        }

        /* Issue the necessary notifications. */
        if (cacheFlush || rowChanged) {
            NotifyPresence(cacheFlush, contributorCursor, discoveryCursor, presence);
        }
        
    } catch(std::exception& e) {
        QCC_DbgPrintf(("InjectPresence Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    return status;
}
QStatus ServiceDatabase::InjectDistance(bool cacheFlush,
                                        DbCursor contributorCursor,
                                        DbCursor discoveryCursor,
                                        double distance,
                                        double accuracy)
{
    QCC_DbgPrintf(("InjectDistance"));

    QStatus status = ER_OK;
    try {            
        /* Get the timestamp to use. */
        uint32_t timestamp = ajn::BusAttachment::GetTimestamp();
        
        /* Make the key. */
        std::string key = MakeRowKey(contributorCursor, discoveryCursor);
        
        /* Get the row to use, creating one if necessary. */
        DbDistanceRow* row;
        
        dbDistanceRowsMutex.Lock(MUTEX_CONTEXT);
        std::unordered_map<std::string, DbDistanceRow*>::iterator it = dbDistanceRows.find(key);
        if (it != dbDistanceRows.end()) {
            row = it->second;
        } else {
            row = new DbDistanceRow(contributorCursor, discoveryCursor);               
            dbDistanceRows.emplace(key, row);
        }
        dbDistanceRowsMutex.Unlock();
        
        /* Inject the distance information.  If the row is change, update the columns.*/
        bool rowChanged = row->SetDistance(timestamp, distance, accuracy);
        if (rowChanged) {
            InjectDistanceColumn(contributorCursor, discoveryCursor);
            InjectDistanceColumn(discoveryCursor, contributorCursor);
        }

        /* Issue the necessary notifications. */
        if (cacheFlush || rowChanged) {
            NotifyDistance(cacheFlush, contributorCursor, discoveryCursor, distance);
        }
        
    } catch(std::exception& e) {
        QCC_DbgPrintf(("InjectDistance Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    return status;
}
QStatus ServiceDatabase::InjectPosition(bool cacheFlush,
                                        DbCursor contributorCursor,
                                        DbCursor discoveryCursor,
                                        Position position,
                                        double accuracy)
{
    QCC_DbgPrintf(("InjectPosition"));

    QStatus status = ER_OK;
    try {            
        /* Get the timestamp to use. */
        uint32_t timestamp = ajn::BusAttachment::GetTimestamp();
        
        /* Make the key. */
        std::string key = MakeRowKey(contributorCursor, discoveryCursor);
        
        /* Get the row to use, creating one if necessary. */
        DbPositionRow* row;
        
        dbPositionRowsMutex.Lock(MUTEX_CONTEXT);
        std::unordered_map<std::string, DbPositionRow*>::iterator it = dbPositionRows.find(key);
        if (it != dbPositionRows.end()) {
            row = it->second;
        } else {
            row = new DbPositionRow(contributorCursor, discoveryCursor);               
            dbPositionRows.emplace(key, row);
        }
        dbPositionRowsMutex.Unlock();
        
        /* Inject the position information.  If the row is change, update the columns.*/
        bool rowChanged = row->SetPosition(timestamp, position, accuracy);
        if (rowChanged) {
            InjectPositionColumn(contributorCursor, discoveryCursor);
            InjectPositionColumn(discoveryCursor, contributorCursor);
        }

        /* Issue the necessary notifications. */
        if (cacheFlush || rowChanged) {
            NotifyPosition(cacheFlush, contributorCursor, discoveryCursor, position);
        }
        
    } catch(std::exception& e) {
        QCC_DbgPrintf(("InjectPosition Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    return status;
}
QStatus ServiceDatabase::CachePresence(DbCache& cache)
{
    QStatus status = ER_OK;

    dbPresenceTablesMutex.Lock(MUTEX_CONTEXT);
    try{
        /* Look at the presence tables to determine items to cache. */
        std::unordered_map<DbCursor, DbTable*>::iterator it;
        for (it=dbPresenceTables.begin(); it != dbPresenceTables.end(); it++) {
            CacheItem(it->first, cache);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("CachePresence Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    dbPresenceTablesMutex.Unlock();

    return status;
}    
QStatus ServiceDatabase::CacheDistance(DbCache& cache)
{
    QCC_DbgPrintf(("CacheDistance - %d", cache.GetRefDevice()));

    QStatus status = ER_OK;

    dbDistanceTablesMutex.Lock(MUTEX_CONTEXT);
    try{
        /* Look at the reference device distance columns to determine items to cache. */
        std::unordered_map<DbCursor, DbTable*>::iterator it = dbDistanceTables.find(cache.GetRefDevice());
        DbTable* dbTable = (it != dbDistanceTables.end()) ? it->second : NULL;
        if (dbTable != NULL) {
            DbCursor column;
            for (int position = 0; (column = dbTable->GetColumn(position)) != 0; position++) {
                CacheItem(column, cache);
            }
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("CacheDistance Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    dbDistanceTablesMutex.Unlock();

    return status;
}    
QStatus ServiceDatabase::CachePosition(DbCache& cache)
{
    QStatus status = ER_OK;

    dbPositionTablesMutex.Lock(MUTEX_CONTEXT);
    try{
        /* Look at the position tables to determine items to cache. */
        std::unordered_map<DbCursor, DbTable*>::iterator it;
        for (it=dbPositionTables.begin(); it != dbPositionTables.end(); it++) {
            CacheItem(it->first, cache);
        }

    } catch(std::exception& e) {
        QCC_DbgPrintf(("CachePosition Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    dbPositionTablesMutex.Unlock();

    return status;
}
QStatus ServiceDatabase::CacheRefresh(DbCursor cursor, DbCache& cache)
{
    QStatus status = ER_OK;

    try{
        /* Flush any cache entries with a matching cursor. */
        std::vector<DbEntity>::iterator it = cache.entities.begin();
        while (it != cache.entities.end()) {
            if ((cursor == it->deviceCursor) || (cursor == it->phraseCursor)) {
                it = cache.entities.erase(it, it + 1);
            } else {
                it++;
            }
        }

        /* Re-cache using the supplied cursor. */
        CacheItem(cursor, cache);

    } catch(std::exception& e) {
        QCC_DbgPrintf(("CacheRefresh Exception: %s\n", e.what())); 
        status = ER_FAIL;
    }
    return status;
} 
bool ServiceDatabase::CacheMatch(DbCursor cursor, DbCache& cache)
{
    bool isMatch = false;
    try {
        isMatch = ((cursor == cache.GetRefDevice()) || (cursor == cache.GetRefPhrase()));

        if (!isMatch) {
            DbItem* item = FindItem(cursor);

            if (item != NULL) {
                isMatch = cache.IsMatch(item->GetSignature());
            }
        }
        
    } catch(std::exception& e) {
        QCC_DbgPrintf(("CacheMatch Exception: %s\n", e.what())); 
    }

    QCC_DbgPrintf(("CacheMatch - %s", (isMatch ? "match" : "no-match")));
    return isMatch;
}
QStatus ServiceDatabase::PresenceEntity(int position, DbCache& cache, ajn::MsgArg& msgArg)
{
    QStatus status = ((position >= 0) && (position < (int)cache.entities.size())) ? ER_OK : ER_FAIL;

    if (status == ER_OK) {
        try {
            /* Get the cached entity information. */
            DbEntity& entity = cache.entities.at(position);
            DbItem* deviceItem = FindItem(entity.deviceCursor);
            DbItem* phraseItem = FindItem(entity.phraseCursor);

            /* Find the table to use. */
            dbPresenceTablesMutex.Lock(MUTEX_CONTEXT);
            std::unordered_map<DbCursor, DbTable*>::iterator it = dbPresenceTables.find(entity.deviceCursor);
            DbTable* dbTable = (it != dbPresenceTables.end()) ? it->second : NULL;
            dbPresenceTablesMutex.Unlock();

            /* Use the table columns to find the presence rows; see if any have the device marked as present. */
            bool isPresent = false;

            if (dbTable != NULL) {
                DbCursor column;
                for (int position = 0; !isPresent && ((column = dbTable->GetColumn(position)) != 0); position++) {
                    /* Get the row key to use. */
                    std::string key = MakeRowKey(entity.deviceCursor, column);

                    /* Get the row. */ 
                    dbPresenceRowsMutex.Lock(MUTEX_CONTEXT);
                    std::unordered_map<std::string, DbPresenceRow*>::iterator it = dbPresenceRows.find(key);
                    DbPresenceRow* row = (it != dbPresenceRows.end()) ? it->second : NULL;
                    dbPresenceRowsMutex.Unlock();

                    /* Get the presence information. */
                    isPresent = (row != NULL) ? row->GetPresence() : false;
                }
            }

            /* Setup the argument. */
            if ((deviceItem != NULL) && (phraseItem != NULL)) {
                msgArg.Set("((ss)b)",
                           deviceItem->GetSignature().c_str(),
                           phraseItem->GetSignature().c_str(),
                           isPresent);
            } else {
                status = ER_FAIL;
            }                  
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("PresenceEntity Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }        
    return status;
}  
QStatus ServiceDatabase::PresenceEntities(DbCache& cache, ajn::MsgArg& msgArg)
{
    int entityCount = (int)cache.entities.size();
    ajn::MsgArg* entityArgs = NULL;
    QStatus status = (entityCount > 0) ? ER_OK : ER_LS_NO_MATCHES;

    if (status == ER_OK) {
        try {
            entityArgs = new ajn::MsgArg[entityCount];
            for(int pos = 0; pos < entityCount; pos++) {
                entityArgs[pos].typeId = ajn::ALLJOYN_INVALID;
                entityArgs[pos].SetOwnershipFlags(0);
            }
            for (int pos = 0; (status == ER_OK) && (pos < entityCount); pos++) {
                status = PresenceEntity(pos, cache, entityArgs[pos]);
            }
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("PresenceEntities Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }

    if (status == ER_OK) {
        entityArgs->SetOwnershipFlags(ajn::MsgArg::OwnsArgs, false);
        msgArg.Set("a((ss)b)", entityCount, entityArgs);
    } else {
        delete[] entityArgs;
    }        
    return status;
}    
QStatus ServiceDatabase::DistanceEntity(int position, DbCache& cache, ajn::MsgArg& msgArg)
{
    QStatus status = ((position >= 0) && (position < (int)cache.entities.size())) ? ER_OK : ER_FAIL;

    if (status == ER_OK) {
        try {
            /* Get the cached entity information. */
            DbEntity& entity = cache.entities.at(position);
            DbItem* deviceItem = FindItem(entity.deviceCursor);
            DbItem* phraseItem = FindItem(entity.phraseCursor);
            
            /* Get the row to use. */
            std::string key = MakeRowKey(cache.GetRefDevice(), entity.deviceCursor);
            
            dbDistanceRowsMutex.Lock(MUTEX_CONTEXT);
            std::unordered_map<std::string, DbDistanceRow*>::iterator it = dbDistanceRows.find(key);
            DbDistanceRow* row = (it != dbDistanceRows.end()) ? it->second : NULL;
            dbDistanceRowsMutex.Unlock();
            
            /* Setup the argument. */
            if ((deviceItem != NULL) && (phraseItem != NULL) && (row != NULL)) {
                msgArg.Set("((ss)dd)",
                           deviceItem->GetSignature().c_str(),
                           phraseItem->GetSignature().c_str(),
                           row->GetDistance(),
                           row->GetAccuracy());
            } else {
                status = ER_FAIL;
            }                  
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("DistanceEntity Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }        
    return status;
}   
/* *** TBD - Look at all columns to see which accuracy to report. */
QStatus ServiceDatabase::DistanceEntities(DbCache& cache, ajn::MsgArg& msgArg)
{
    int entityCount = (int)cache.entities.size();
    ajn::MsgArg* entityArgs = NULL;
    QStatus status = (entityCount > 0) ? ER_OK : ER_LS_NO_MATCHES;

    if (status == ER_OK) {
        try {
            entityArgs = new ajn::MsgArg[entityCount]; 
            for(int pos = 0; pos < entityCount; pos++) {
                entityArgs[pos].typeId = ajn::ALLJOYN_INVALID;
                entityArgs[pos].SetOwnershipFlags(0);
            }       
            for (int pos = 0; (status == ER_OK) && (pos < entityCount); pos++) {
                status = DistanceEntity(pos, cache, entityArgs[pos]);
            }
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("DistanceEntities Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }

    if (status == ER_OK) {
        entityArgs->SetOwnershipFlags(ajn::MsgArg::OwnsArgs, false);
        msgArg.Set("a((ss)dd)", entityCount, entityArgs);
    } else {
        delete[] entityArgs;
    }        
    return status;
}  
QStatus ServiceDatabase::GeofenceEntity(bool withinFenceline, DbEntity &entity, ajn::MsgArg& msgArg)
{
    DbItem* deviceItem = FindItem(entity.deviceCursor);
    DbItem* phraseItem = FindItem(entity.phraseCursor);    
    QStatus status = ((deviceItem != NULL) && (phraseItem != NULL)) ? ER_OK : ER_FAIL;

    if (status == ER_OK) {
        try {
            status = msgArg.Set("((ss)b)",
                                deviceItem->GetSignature().c_str(),
                                phraseItem->GetSignature().c_str(),
                                withinFenceline);
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("GeofenceEntity Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }        
    return status;
} 
QStatus ServiceDatabase::GeofenceEntity(int position, double fenceline, DbCache& cache, ajn::MsgArg& msgArg)
{
    QCC_DbgPrintf(("GeofenceEntity - %f", fenceline));

    QStatus status = ((position >= 0) && (position < (int)cache.entities.size())) ? ER_OK : ER_FAIL;

    if (status == ER_OK) {
        try {
            /* Get the cached entity information. */
            DbEntity& entity = cache.entities.at(position);
            DbItem* deviceItem = FindItem(entity.deviceCursor);
            DbItem* phraseItem = FindItem(entity.phraseCursor);
            
            /* Get the row to use. */
            std::string key = MakeRowKey(cache.GetRefDevice(), entity.deviceCursor);
            
            dbDistanceRowsMutex.Lock(MUTEX_CONTEXT);
            std::unordered_map<std::string, DbDistanceRow*>::iterator it = dbDistanceRows.find(key);
            DbDistanceRow* row = (it != dbDistanceRows.end()) ? it->second : NULL;
            dbDistanceRowsMutex.Unlock();
            
            /* Setup the argument. */
            if ((deviceItem != NULL) && (phraseItem != NULL) && (row != NULL)) {
                msgArg.Set("((ss)b)",
                           deviceItem->GetSignature().c_str(),
                           phraseItem->GetSignature().c_str(),
                           (bool)(std::fabs(row->GetDistance()) <= std::fabs(fenceline)));
            } else {
                status = ER_FAIL;
            }                  
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("GeofenceEntity Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }        
    return status;
}   
QStatus ServiceDatabase::GeofenceEntities(double fenceline, DbCache& cache, ajn::MsgArg& msgArg)
{
    int entityCount = (int)cache.entities.size();
    ajn::MsgArg* entityArgs = NULL;
    QStatus status = (entityCount > 0) ? ER_OK : ER_LS_NO_MATCHES;

    if (status == ER_OK) {
        try {
            entityArgs = new ajn::MsgArg[entityCount]; 
            for(int pos = 0; pos < entityCount; pos++) {
                entityArgs[pos].typeId = ajn::ALLJOYN_INVALID;
                entityArgs[pos].SetOwnershipFlags(0);
            }       
            for (int pos = 0; (status == ER_OK) && (pos < entityCount); pos++) {
                status = GeofenceEntity(pos, fenceline, cache, entityArgs[pos]);
            }
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("GeofenceEntities Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }

    if (status == ER_OK) {
        entityArgs->SetOwnershipFlags(ajn::MsgArg::OwnsArgs, false);
        msgArg.Set("a((ss)b)", entityCount, entityArgs);
    } else {
        delete[] entityArgs;
    }        
    return status;
}   
QStatus ServiceDatabase::PositionEntity(int position, DbCache& cache, ajn::MsgArg& msgArg)
{
    QStatus status = ((position >= 0) && (position < (int)cache.entities.size())) ? ER_OK : ER_FAIL;

    if (status == ER_OK) {
        try {
            /* Get the cached entity information. */
            DbEntity& entity = cache.entities.at(position);
            DbItem* deviceItem = FindItem(entity.deviceCursor);
            DbItem* phraseItem = FindItem(entity.phraseCursor);
            
            /* Get the row to use. */
            std::string key = MakeRowKey(cache.GetRefDevice(), entity.deviceCursor);
            
            dbPositionRowsMutex.Lock(MUTEX_CONTEXT);
            std::unordered_map<std::string, DbPositionRow*>::iterator it = dbPositionRows.find(key);
            DbPositionRow* row = (it != dbPositionRows.end()) ? it->second : NULL;
            dbPositionRowsMutex.Unlock();
            
            /* Setup the argument. */
            if ((deviceItem != NULL) && (phraseItem != NULL) && (row != NULL)) {
                Position posInfo = row->GetPosition();
                msgArg.Set("((ss)(sdddd))",
                           deviceItem->GetSignature().c_str(),
                           phraseItem->GetSignature().c_str(),
                           posInfo.origin.c_str(),
                           posInfo.xCoordinate,
                           posInfo.yCoordinate,
                           posInfo.zCoordinate,
                           row->GetAccuracy());
            } else {
                status = ER_FAIL;
            }                  
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("PositionEntity Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }        
    return status;
}   
QStatus ServiceDatabase::PositionEntities(DbCache& cache, ajn::MsgArg& msgArg)
{
    int entityCount = (int)cache.entities.size();
    ajn::MsgArg* entityArgs = NULL;
    QStatus status = (entityCount > 0) ? ER_OK : ER_LS_NO_MATCHES;

    if (status == ER_OK) {
        try {
            entityArgs = new ajn::MsgArg[entityCount]; 
            for(int pos = 0; pos < entityCount; pos++) {
                entityArgs[pos].typeId = ajn::ALLJOYN_INVALID;
                entityArgs[pos].SetOwnershipFlags(0);
            }       
            for (int pos = 0; (status == ER_OK) && (pos < entityCount); pos++) {
                status = PositionEntity(pos, cache, entityArgs[pos]);
            }
            
        } catch(std::exception& e) {
            QCC_DbgPrintf(("PositionEntities Exception: %s\n", e.what())); 
            status = ER_FAIL;
        }
    }

    if (status == ER_OK) {
        entityArgs->SetOwnershipFlags(ajn::MsgArg::OwnsArgs, false);
        msgArg.Set("a((ss)(sdddd))", entityCount, entityArgs);
    } else {
        delete[] entityArgs;
    }        
    return status;
}  



