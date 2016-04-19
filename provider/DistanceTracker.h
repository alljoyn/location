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
#ifndef DISTANCE_TRACKER_H_
#define DISTANCE_TRACKER_H_


class DistanceTracker : public LsTracker, public ajn::BusObject, public DbListener
{
private: 
    qcc::Mutex cacheMutex;
   
    DbDevice* trackerDevice;
    qcc::String trackerSignature;
    double trackerDelta;

    QStatus Get(const char* ifcName, const char* propName, ajn::MsgArg& val);
    void KeepAlive(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void Query(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    bool CacheChanged(qcc::String signature, uint32_t timestamp, double range);
    void TrackingSignal(qcc::String signature, double range);
    
public:
    DistanceTracker(qcc::String path, ajn::BusAttachment* msgBus, LsDatabase* lsDb, LsManager* lsMan);
    ~DistanceTracker();
    QStatus Start(ajn::Message& msg);
    void DetectEvent(qcc::String sensorSignature, 
                     qcc::String entitySignature,
                     uint32_t timestamp, 
                     bool detect);
    void RangeEvent(qcc::String sensorSignature, 
                    qcc::String entitySignature,
                    uint32_t timestamp, 
                    double range);
};             


#endif /* DISTANCE_TRACKER_H_ */
