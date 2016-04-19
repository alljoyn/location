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
#ifndef LS_TRACKER_H_
#define LS_TRACKER_H_


class LsTracker
{
private:
    qcc::Mutex trackerMutex;

    uint32_t ttlPeriodMs;
    uint32_t ttlSignalMs;
    uint32_t ttlTimestamp;

protected:
    uint16_t trackerVersion;
    qcc::String trackerPath;

    const ajn::InterfaceDescription* intf;
    ajn::BusAttachment* msgBus;
    ajn::BusObject* busObject;
    LsDatabase* lsDb;
    LsManager* lsMan;

    DbFilter* dbFilter;
    DbResult* dbCache;
    
public:

    LsTracker(uint16_t trackerVersion, qcc::String trackerPath);
    ~LsTracker();
    bool LifetimeRemaining();
    void LifetimeExtension();
};             


#endif /* LS_TRACKER_H_ */
