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
#ifndef PRESENCE_CONTRIBUTOR_H_
#define PRESENCE_CONTRIBUTOR_H_


class PresenceContributor : public ServiceObject
{
private:
    qcc::Mutex limitFilterMutex;
    DbFilter* limitFilter;

    QStatus Get(const char* ifcName, const char* propName, ajn::MsgArg& val);
    void UpdatePresenceInfo(const ajn::InterfaceDescription::Member* member, ajn::Message& msg); 
    void LimitContribution(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);       
public:

    PresenceContributor(ajn::BusAttachment* msgBus, ServiceDatabase* svcDb);
    ~PresenceContributor();
    QStatus Start();
};             


#endif /* PRESENCE_CONTRIBUTOR_H_ */
