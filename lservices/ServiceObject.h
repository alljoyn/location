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
#ifndef SERVICE_OBJECT_H_
#define SERVICE_OBJECT_H_


class ServiceObject : public ajn::BusObject
{
protected:
    ajn::BusAttachment* msgBus;
    ServiceDatabase* svcDb;

    const ajn::InterfaceDescription* svcIntf;

    qcc::String Lowercase(qcc::String myString);
    QStatus ConfigureFilter(int entityParserMember,
                            int entityListMember,
                            const ajn::MsgArg& filterArg,
                            DbFilter& dbFilter);
    
public:
    ServiceObject(const char* path, ajn::BusAttachment* msgBus, ServiceDatabase* svcDb);
    virtual ~ServiceObject();
    virtual QStatus Start();
    virtual QStatus Start(ajn::Message& msg);
    virtual void Check();
};             


#endif /* SERVICE_OBJECT_H_ */
