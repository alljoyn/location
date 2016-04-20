/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
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


namespace org_alljoyn_locationservices { namespace Locationservices {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.Locationservices";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.Locationservices\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"EntityPresence\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <arg name=\"sensor\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"isPresent\" type=\"b\" direction=\"in\"/>"
    "  </method>"
    "  <method name=\"EntityRange\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <arg name=\"sensor\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"range\" type=\"x\" direction=\"in\"/>"
    "  </method>"
    "  <method name=\"PresenceSubscribe\">"
    "    <arg name=\"filterParser\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterList\" type=\"a(ss)\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"DistanceSubscribe\">"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterParser\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterList\" type=\"a(ss)\" direction=\"in\"/>"
    "    <arg name=\"delta\" type=\"x\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"ProximitySubscribe\">"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterParser\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filterList\" type=\"a(ss)\" direction=\"in\"/>"
    "    <arg name=\"proximity\" type=\"x\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "</interface>";
}}


namespace org_alljoyn_locationservices { namespace PresenceTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.PresenceTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.PresenceTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssb)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <arg name=\"entity\" type=\"(ssb)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "</interface>";
}}


namespace org_alljoyn_locationservices { namespace DistanceTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.DistanceTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.DistanceTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssx)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <arg name=\"entity\" type=\"(ssx)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "</interface>";
}}

namespace org_alljoyn_locationservices { namespace ProximityTracker {
const char* const InterfaceName = 
    "org.alljoyn.locationservices.ProximityTracker";
const char* const InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.ProximityTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssb)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <arg name=\"entity\" type=\"(ssb)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
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
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"o\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ss)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "  <signal name=\"Tracking\">"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"out\"/>"
    "  </signal>"
    "  <signal name=\"EndOfLife\">"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"
    "  <method name=\"KeepAlive\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "</interface>";
}}


#endif /* LS_STD_H_ */