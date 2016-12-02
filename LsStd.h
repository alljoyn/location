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
#ifndef LS_STD_H_
#define LS_STD_H_

/* Place holder until we have a way of integrating into Status.h */
#define ER_LS_NO_MATCHES 0x3030

namespace org_alljoyn_locationservices {
const char * const ServiceName =
    "org.alljoyn.locationservices";
}
namespace org_alljoyn_locationservices { namespace Locationservices {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.Locationservices";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.Locationservices\">"
    "  <description language=\"en\">LSI</description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"EntityPresence\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <description language=\"en\">Entity Presence</description>"
    "    <arg name=\"sensor\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"isPresent\" type=\"b\" direction=\"in\"/>"
    "  </method>"
    "  <method name=\"EntityRange\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <description language=\"en\">Entity Range</description>"
    "    <arg name=\"sensor\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"range\" type=\"d\" direction=\"in\"/>"
    "  </method>"
    "  <method name=\"PresenceSubscribe\">"
    "    <description language=\"en\">Presence Tracker Subscribe</description>"
    "    <arg name=\"filterParser\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterList\" type=\"a(ss)\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"DistanceSubscribe\">"
    "    <description language=\"en\">Distance Tracker Subscribe</description>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterParser\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterList\" type=\"a(ss)\" direction=\"in\"/>"
    "    <arg name=\"delta\" type=\"d\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"ProximitySubscribe\">"
    "    <description language=\"en\">Proximity Tracker Subscribe</description>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterParser\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterList\" type=\"a(ss)\" direction=\"in\"/>"
    "    <arg name=\"proximity\" type=\"d\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "</interface>";
}}


namespace org_alljoyn_locationservices { namespace PresenceTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.PresenceTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.PresenceTracker\">"
    "  <description language=\"en\">Presence Tracker</description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssb)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <description language=\"en\">"
    "\"This interface contains methods for applications to track presense\""
    "</description>"
    "  <signal name=\"Tracking\" sessioncast=\"true\">"
    "    <description language=\"en\">Tracking Signal</description>"
    "    <arg name=\"entity\" type=\"(ssb)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <description language=\"en\">EOL Signal</description>"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "  <method name=\"Query\">"
    "    <arg name=\"matches\" type=\"a(ssb)\" direction=\"out\"/>"
    "  </method>"
    "</interface>";
}}


namespace org_alljoyn_locationservices { namespace DistanceTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.DistanceTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.DistanceTracker\">"
    "  <description language=\"en\">Distance Tracker</description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <description language=\"en\">"
    "\"This interface contains methods for applications to subscribe to location servicestrack distance\""
    "</description>"
    "  <property name=\"Matches\" type=\"a(ssd)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <description language=\"en\">Tracking Signal</description>"
    "    <arg name=\"entity\" type=\"(ssd)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <description language=\"en\">EOL Signal</description>"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "  <method name=\"Query\">"
    "    <arg name=\"matches\" type=\"a(ssd)\" direction=\"out\"/>"
    "  </method>"
    "</interface>";
}}

namespace org_alljoyn_locationservices { namespace ProximityTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.ProximityTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.ProximityTracker\">"
    "  <description language=\"en\">Proximity Tracker</description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssb)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <description language=\"en\">Tracking Signal</description>"
    "    <arg name=\"entity\" type=\"(ssb)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <description language=\"en\">EOL Signal</description>"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "</interface>";
}}

namespace org_alljoyn_locationservices { namespace LocationTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.LocationTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.LocationTracker\">"
    "  <description language=\"en\">Location Tracker</description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ss)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <description language=\"en\">Tracking Signal</description>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <description language=\"en\">EOL Signal</description>"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "</interface>";
}}


#endif /* LS_STD_H_ */