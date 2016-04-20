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
#ifndef DISTANCE_TRACKER_H_
#define DISTANCE_TRACKER_H_


class DistanceTracker : public TrackerObject 
{
private:
    ajn::MsgArg* filterArg;
    double deltaArg;

    qcc::Mutex attributeMutex;
    std::unordered_map<DbCursor, double> attributeMap;

    QStatus Get(const char* ifcName, const char* propName, ajn::MsgArg& val);
    void Matches(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);   
    void KeepAlive(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void SignalChanges(bool cacheFlush,
                       DbCursor contributorCursor, 
                       DbCursor discoveryCursor,
                       double distance);
    bool PostAttribute(DbCursor cursor, double distance);
public:

    DistanceTracker(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb);
    ~DistanceTracker();
    QStatus Start(ajn::Message& msg);
    void DistanceEvent(bool cacheFlush,
                       DbCursor contributorCursor, 
                       DbCursor discoveryCursor,
                       double distance);
};             


#endif /* DISTANCE_TRACKER_H_ */
