/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
 *    
 *    SPDX-License-Identifier: Apache-2.0
 *    
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *    
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *    
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *    
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
******************************************************************************/
#ifndef SERVICE_MANAGER_H_
#define SERVICE_MANGER_H_


class ServiceManager : public ajn::SessionPortListener 
{
private:
    const char* APP_GUID = "7f0abc4e-932a-4d94-bcba-986bdf24b2aa";
    const char* DEVICE_GUID = "9ad62045-611d-485d-ab53-0017c28a02bb";
    const char* SERVICE_NAME = "org.alljoyn.locationservices";

    ajn::BusAttachment* msgBus;

    ajn::AboutData aboutData;
    ajn::AboutObj* aboutObj;

    ServiceDatabase* svcDb;
    
    qcc::Mutex svcObjectsMutex;
    std::vector<ServiceObject*> svcObjects;

    uint32_t apiTimestamp;
    
    QStatus ApiInclude(ServiceObject* svcObject);
    bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);
    void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);    
public:
    ServiceManager(ajn::BusAttachment* msgBus);
    virtual ~ServiceManager();
    QStatus StartApi();
    void CheckApi();
};             


#endif /* SERVICE_MANAGER_H_ */