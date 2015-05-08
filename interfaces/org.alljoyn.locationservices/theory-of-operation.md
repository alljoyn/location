# org.allseen.LocationServices version 1

## Theory of Operation
Location Services encompasses four types of service, presence, proximity, location and containment.
Each of these services attempts to locate a device and relay information about the device to the Alljoyn network.
The device may or may not be connected to the Alljoyn
network. Location services can proxy the location information of non-attached devices to the Alljoyn network.
Examples of entities are mobile phones, lights, beacons, smart watches, the device hosting location service, etc...
Devices tracked by the location service are called entities.
Location services allow devices to add modify and delete information held by location services, as an example, remote beacon listeners
can be installed to update the location information held by location services. When a remote device adds information the location services
keeps a reference to that device.
A device may feed information into location services without necessarily hosting the service itself. These devices are called contributing devices.
Location services provide both a Query(pull) mechanism and a Publish/Subscribe(push) mechanism to obtain information about devices.
Location services returns current positional information.

ENTITIES
An entity is the representation of a device that you want locate. A device on the Alljoyn network may not be an entity.
An entity can be an active entity that can add its location information to the location services such as a mobile phone, or it can be a passive entity such as a beacon that does not participate in the alljoyn network.
Entities can be known or anonymous. A known entity has registered with the location service. An anonymous
entity has not. An entity may not participating in the alljoyn network, examples are beacons, NFC tags, fitbits etc..

GROUPS
A group is a dictionary entry that maps a group name to a list of entities.
Groups allow code to be written without knowing the identity of all of the entities. Applications, active entities and contributing devices can add or remove entities from a group.
Groups allow easy management of large numbers of entities.
Groups allow applications to add entities to a group for use by other applications.
A privacy policy can be applied to a group.
Groups are a property of the location services interface.
A property changed signal is generated whenever group membership changes.

PRESENCE
The presence service announces the entry or exit of an entity to the proximal network.
The presence service can be queried to determine if an entity is or is not present.
Users of the presence service can subscribe to changes in presence of known, anonymous, groups, or all entities.
An entity is deemed not to be present when all of the contributing devices declare it is not present.
An entity is deemed to be present if at least one contributing device claims the entity is present.
The presence service is different from the About service in that entities may not be running Alljoyn services (e.g. beacons...).

PROXIMITY
The proximity service provides data on the relative distance between two entities.
Users of this service can query for the distance between a reference entity and i) another entity, ii) a group of entities, or iii) all entities within a specified distance.
Users of this service can subscribe to notifications when an entity comes within a specified distance of another entity, or any member of a group.
or get a notification if an entity moves a specified distance in regards to another entity.
A contributing device for the proximity service must also be an entity since proximity is the distance between two entities.

LOCATION
The location service provides two  types of location data. The absolute location of an entity is described by WGS 84 data.  The location can alternatively specify the location of an entity with respect to an origin is described by origin, X, Y and Z coordinates specified in meters. The origin is a string which meaning determined locally. Users of this service can query for the location of an entity or a group of entities. Users of this service can also subscribe for location information of an entity or a group of entities and receive signals about change in their location.
Locations sensors (contributing devices) may report either absolute or relative location information.
The horizontal and vertical accuracy of the location data is also returned. This is accuracy information is supplied by the contributing device.

CONTAINMENT
The containment services provides information about which entities reside within a container and which container and entity is within. The details are left for a future release.

SECURITY

PRIVACY
The service provides the ability to apply a privacy policy to an entity.
It is not possible to provide privacy services to anonymous entities.
The only way to prevent anonymous entities from being reporting  is to stop radio transmission.

Privacy concerns include the ability for an application to control the degree to which an entity is reported to applications.
A location service ACL allows or disallows the location service from reporting on an entity. The default ACL is to allow.
The privacy policy consists of 4 ACLs. Allow/Disallow presence, allow/disallow proximity, allow/disallow location and allow/disallow containment.
A policy can be applied to an entity or a group.
An entities or groups policy has a white list and a black list for each of the services. The white/black lists contain the identity of a consuming applications (Need to figure out what this is).
An entity has a default ACL that can be allow, disallow or callback.  the application that registered the entity into location services.
A group has a default ACL that is allow or disallow.

If a privacy policy is applied to an entity, it will override any policy applied to a group that the entity is a member of.


Presence ACL. The presence privacy policy will determine whether the presence of an entity can be announced.
Proximity ACL. The proximity ACL is applied to an entity and another entity or group, it determines whether the proximity of the entity can be announced.
Location ACL. The location privacy policy will determine whether the location of an entity can be announced.
Containment ACL. - deferred

SCENARIOS


