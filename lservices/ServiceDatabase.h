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
#ifndef SERVICE_DATABASE_H_
#define SERVICE_DATABASE_H_


typedef uint32_t DbCursor;


class DbIdentity
{
private:
    DbCursor cursor;
    const qcc::String* signature;
public:
    DbIdentity(const qcc::String& signature);
    virtual ~DbIdentity();
    DbCursor GetCursor();
    const qcc::String& GetSignature();
};

class DbItem : public DbIdentity
{
private:
    qcc::Mutex aliasMutex;
    std::vector<DbIdentity*> aliasList;  
public:
    DbItem(const qcc::String& signature);
    virtual ~DbItem();
    bool PutAlias(DbIdentity* alias);
    DbIdentity* GetAlias(int position);
    DbIdentity* GetAlias(qcc::String& signature);
};

class DbEntity
{
public:
    DbCursor deviceCursor;
    DbCursor phraseCursor;
    DbEntity(DbCursor deviceCursor, DbCursor phraseCursor);
};

class DbParser
{
public:
    std::regex deviceExpression;
    std::regex phraseExpression;
    DbParser(const qcc::String& uidExp, const qcc::String& descriptorExp);
};

class DbFilter
{
private:
    DbCursor refDevice;
    DbCursor refPhrase;

    qcc::Mutex parsersMutex;
    std::vector<DbParser> parsers;
public:
    DbFilter(DbCursor refDevice, DbCursor refPhrase);
    virtual ~DbFilter();
    DbCursor GetRefDevice();
    DbCursor GetRefPhrase();
    void AddParser(const qcc::String& uidExp, const qcc::String& descriptorExp);
    bool IsMatch(const qcc::String& uid);
    bool IsMatch(const qcc::String& uid, const qcc::String& descriptor);
    bool IsMatch(DbIdentity* device, DbIdentity* phrase);
};

class DbCache : public DbFilter
{
public:
    std::vector<DbEntity> entities;

    DbCache(DbCursor refDevice, DbCursor refPhrase);
    virtual ~DbCache();
};

class DbRow
{
protected:
    uint32_t timestamp;
    const DbCursor contributorCursor;
    const DbCursor discoveryCursor;
public:
    DbRow(DbCursor contributorCursor, DbCursor discoveryCursor);
    virtual ~DbRow();
    DbCursor GetContributorCursor();
    DbCursor GetDiscoveryCursor();
    DbCursor GetLinkedCursor(DbCursor cursor);
};

class DbPresenceRow : public DbRow
{
private:
    qcc::Mutex presenceMutex;
    bool isPresent;
public:
    DbPresenceRow(DbCursor contributorCursor, DbCursor discoveryCursor);
    virtual ~DbPresenceRow();
    bool SetPresence(uint32_t timestamp, bool isPresent);
    bool GetPresence();
};

class DbDistanceRow : public DbRow
{
private:
    qcc::Mutex distanceMutex;
    double distance;
    double accuracy;
public:
    DbDistanceRow(DbCursor contributorCursor, DbCursor discoveryCursor);
    virtual ~DbDistanceRow();
    bool SetDistance(uint32_t timestamp, double distance, double accuracy); 
    double GetDistance();
    double GetAccuracy();
};

class DbPositionRow : public DbRow
{
private:
    qcc::Mutex positionMutex;
    Position position;
    double accuracy;
public:
    DbPositionRow(DbCursor contributorCursor, DbCursor discoveryCursor);
    virtual ~DbPositionRow();
    bool SetPosition(uint32_t timestamp, 
                     Position position, 
                     double accuracy);
    Position GetPosition();
    double GetAccuracy();
};

class DbTable
{
private:
    qcc::Mutex columnsMutex;
    std::vector<DbCursor> columns;
public:
    DbTable();
    virtual ~DbTable();
    void PutColumn(DbCursor cursor);
    DbCursor GetColumn(int position);
};
                
class DbListener 
{
public:
    DbCursor cursor;

    DbListener();
    virtual ~DbListener();
    DbCursor GetCursor();
    virtual void PresenceEvent(bool cacheFlush, 
                               DbCursor contributorCursor, 
                               DbCursor discoveryCursor,
                               bool presence);
    virtual void DistanceEvent(bool cacheFlush, 
                               DbCursor contributorCursor, 
                               DbCursor discoveryCursor,
                               double distance);
    virtual void PositionEvent(bool cacheFlush, 
                               DbCursor contributorCursor, 
                               DbCursor discoveryCursor,
                               Position position);
};

class ServiceDatabase
{
private:
    /* One entry per device (unique id). */
    qcc::Mutex dbDevicesMutex;
    std::unordered_map<std::string, DbItem*> dbDevices;

    /* One entry per phrase (descriptor). */
    qcc::Mutex dbPhrasesMutex;
    std::unordered_map<std::string, DbItem*> dbPhrases;

    /* One entry per device and phrase cursor. */
    qcc::Mutex dbItemsMutex;
    std::unordered_map<DbCursor, DbItem&> dbItems;

    /* One entry per registered listener - is assigned a cursor. */
    qcc::Mutex dbListenersMutex;
    std::unordered_map<DbCursor, DbListener*> dbListeners;

    /* One entry per contributor/discovery device injected presence. */
    qcc::Mutex dbPresenceRowsMutex;
    std::unordered_map<std::string, DbPresenceRow*> dbPresenceRows;

    /* One entry per contributor/discovery device injected distance. */
    qcc::Mutex dbDistanceRowsMutex;
    std::unordered_map<std::string, DbDistanceRow*> dbDistanceRows;

    /* One entry per contributor/discovery device injected position. */
    qcc::Mutex dbPositionRowsMutex;
    std::unordered_map<std::string, DbPositionRow*> dbPositionRows; 

    /* One entry per device with presence information (including associated device cursors). */
    qcc::Mutex dbPresenceTablesMutex;
    std::unordered_map<DbCursor, DbTable*> dbPresenceTables;

    /* One entry per device with distance information (including associated device cursors). */
    qcc::Mutex dbDistanceTablesMutex;
    std::unordered_map<DbCursor, DbTable*> dbDistanceTables;

    /* One entry per device with position information (including associated device cursors). */
    qcc::Mutex dbPositionTablesMutex;
    std::unordered_map<DbCursor, DbTable*> dbPositionTables;


    void InjectPresenceColumn(DbCursor tableCursor, DbCursor columnCursor);
    void InjectDistanceColumn(DbCursor tableCursor, DbCursor columnCursor);
    void InjectPositionColumn(DbCursor tableCursor, DbCursor columnCursor);

    void NotifyPresence(bool cacheFlush, 
                        DbCursor contributorCursor, 
                        DbCursor discoveryCursor,
                        bool presence);
    void NotifyDistance(bool cacheFlush, 
                        DbCursor contributorCursor, 
                        DbCursor discoveryCursor,
                        double distance);
    void NotifyPosition(bool cacheFlush, 
                        DbCursor contributorCursor, 
                        DbCursor discoveryCursor,
                        Position position);
    
    DbItem* FindItem(DbCursor cursor);
    void CacheItem(DbCursor cursor, DbCache& cache);
public:
    ServiceDatabase();
    virtual ~ServiceDatabase();

    DbCursor MapDevice(const qcc::String& uid, const qcc::String& descriptor);
    DbCursor FindDevice(const qcc::String& uid, const qcc::String& descriptor);
    DbCursor FindPhrase(const qcc::String& descriptor);    

    QStatus RegisterListener(DbListener* listener);
    QStatus DeregisterListener(DbListener* listener);

    QStatus InjectPresence(bool cacheFlush,
                           DbCursor contributorCursor,
                           DbCursor discoveryCursor,
                           bool presence);
    QStatus InjectDistance(bool cacheFlush,
                           DbCursor contributorCursor,
                           DbCursor discoveryCursor,
                           double distance,
                           double accuracy);
    QStatus InjectPosition(bool cacheFlush,
                           DbCursor contributorCursor,
                           DbCursor discoveryCursor,
                           Position position,
                           double accuracy);

    /* ***Cache locking is up to the caller.*** */
    QStatus CachePresence(DbCache& cache);
    QStatus CacheDistance(DbCache& cache);
    QStatus CachePosition(DbCache& cache);
    QStatus CacheRefresh(DbCursor cursor, DbCache& cache);
    bool CacheMatch(DbCursor cursor, DbCache& cache);

    /* ***Cache locking is up to the caller.*** */
    QStatus PresenceEntity(int position, DbCache& cache, ajn::MsgArg& msgArg);
    QStatus PresenceEntities(DbCache& cache, ajn::MsgArg& msgArg);

    /* ***Cache locking is up to the caller.*** */
    QStatus DistanceEntity(int position, DbCache& cache, ajn::MsgArg& msgArg);
    QStatus DistanceEntities(DbCache& cache, ajn::MsgArg& msgArg);

    /* ***Cache locking is up to the caller.*** */
    QStatus GeofenceEntity(bool withinFenceline, DbEntity &entity, ajn::MsgArg& msgArg);
    QStatus GeofenceEntity(int position, double fenceline, DbCache& cache, ajn::MsgArg& msgArg);
    QStatus GeofenceEntities(double fenceline, DbCache& cache, ajn::MsgArg& msgArg);

    /* ***Cache locking is up to the caller.*** */
    QStatus PositionEntity(int position, DbCache& cache, ajn::MsgArg& msgArg);
    QStatus PositionEntities(DbCache& cache, ajn::MsgArg& msgArg);

};            


#endif /* SERVICE_DATABASE_H_ */