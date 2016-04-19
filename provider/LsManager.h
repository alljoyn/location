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
#ifndef LS_MANAGER_H_
#define LS_MANGER_H_

class Locationservices;

class LsManager : public ajn::SessionPortListener 
{
private:
    const char* GUID = "353e370d-e654-4908-8a45-37d3e8d74a5c";

    ajn::BusAttachment* msgBus;

    ajn::AboutData aboutData;
    ajn::AboutObj* aboutObj;
    LsDatabase* lsDb;
    Locationservices* lsObject;
    uint32_t apiTimestamp;
    
    bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);
    void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);
    
public:
    LsManager(ajn::BusAttachment* msgBus);
    ~LsManager();
    QStatus StartApi();
    void CheckApi();
};             


#endif /* LS_MANAGER_H_ */
