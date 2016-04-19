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


LsTracker::LsTracker(uint16_t trackerVersion, qcc::String trackerPath) 
    : trackerMutex(), trackerVersion(trackerVersion), trackerPath(trackerPath)
{
    ttlPeriodMs = 300000;
    ttlSignalMs = 240000;
    ttlTimestamp = ajn::BusAttachment::GetTimestamp();
}

LsTracker::~LsTracker()
{
    return;
}


bool LsTracker::LifetimeRemaining() {
    //QCC_DbgPrintf(("LifetimeRemaining - %ld", ttlTimestamp));

    bool result;

    trackerMutex.Lock(MUTEX_CONTEXT);
    try {
        uint32_t lifetime = ajn::BusAttachment::GetTimestamp() - ttlTimestamp;

        if (lifetime >= ttlPeriodMs) {
            result = false;

        } else if (lifetime >= ttlSignalMs) {
            result = true;
            ttlSignalMs = ttlPeriodMs;

            // This works for Android but not Higgns.
            MsgArg arg("u", ((ttlPeriodMs - lifetime) / 1000));
            busObject->Signal(NULL, 
                              SESSION_ID_ALL_HOSTED, 
                              *(intf->GetMember("EndOfLife")), 
                              &arg, 
                              1);
            QCC_DbgPrintf(("EndOfLife Signal - %s", trackerPath.c_str()));

        } else {
            result = true;
        }

    } catch(std::exception& e) {
        result = true;
        QCC_DbgPrintf(("LifetimeRemaining Exception: %s\n", e.what()));
    }
    trackerMutex.Unlock();

    return result;
}

void LsTracker::LifetimeExtension() {
    QCC_DbgPrintf(("LifetimeExtension - %s", trackerPath.c_str()));

    trackerMutex.Lock(MUTEX_CONTEXT);
    ttlPeriodMs = 300000;
    ttlSignalMs = 240000;
    ttlTimestamp = ajn::BusAttachment::GetTimestamp();
    trackerMutex.Unlock();
}



