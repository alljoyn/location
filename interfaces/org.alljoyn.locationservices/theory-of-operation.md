# org.allseen.LocationServices version 1

## Theory of Operation
Location Services encompasses five types of service, presence, distance, geofence, position and containment.
Each of these services attempts to locate a device and relay information about the device to the Alljoyn network.
The device may or may not be connected to the Alljoynnetwork. 
Location services can proxy the location information of non-attached devices to the Alljoyn network.
Devices tracked by the location service are called entities.
Devices wich inject data into location services are called contributing devices. Every contributing device has an entity which represents that device.
Location services allow contributing devices to add modify and delete information held by location services, as an example, remote beacon listeners
can be installed to update the location information held by location services. When a remote device adds information the location services
keeps a reference to that device.
A device may feed information into location services without necessarily hosting the service itself. These devices are called contributing devices.
Location services provide both a Query(pull) mechanism and a Publish/Subscribe(push) mechanism to obtain information about devices.
Location services returns current positional information.

#### Entities
An entity is the representation of a device. It consists of two strings, a description and a MAC. The description is a human readable string that describes the entity, the MAC is a string representing the device. The MAC may be an IEEE 802.1 physical address, a UUID or other item unique to the device.
A device may have multiple entities registered to it discriminated by the entity description.

An entity may or may not be conntected to the Alljoyn network.
A device on the Alljoyn network may not be an entity.
An entity can be an active entity that can add its location information to the location services such as a mobile phone, or it can be a passive entity such as a beacon that does not participate in the alljoyn network.
Entities can be known or anonymous. A known entity has registered with the location service. Registration of entities is done by the contributing device.
An anonymous entity has not registered with the location service. Anonymous entities have no expectation of privacy.


#### Contributing Devices
A contributing device is any device that can add modify or delete data in location services. Examples of contributing devices are bluetooth sensors, cell phones and NFC sensors.
A contributing device may feed information into location services without necessarily hosting the service itself. 
When injecting data into location services the contributing device must also pass its own enity information. This entity information is used by loation services. For example the presence service will not declare an entity absent until all contributing devices reporting on the entity report the entity absent.
The contributing device calls a mathod in the relavent location services, thus other technology frameworks can contribute location information into location services.

#### Location services
Location services is a service that runs in the proximal network. It receives information on enitities from contributing devices and responds to queries and subscriptions from applications. The service is responsible for managing the trackers created by subscriptions.
Location services also is responsible for applying privacy policies to requests for entity information. (See Privacy).


Support for multiple co-operative instances of location services is not currently specified.

#### Filters
A filter is used by applications to retrieve information about entities tracked by location services. A filter can query a single entity, a list of entities or a regular expression parser. The use of a regular expression allows applications to query for entities without knowing about them apriori. 
The regular expression parser is an entity containing a regular expresson in the entity description and/or the entity mac. The filter is applied to the location service database an only those entities that match the filter are reported on.
Filters are uesd for queries in a one time requiest or subscriptions for continual update on the location data of enitites.
Filters are also used for the application of privacy policies.

#### Trackers
A Tracker is an Alljoyn Bus Object that is the results of a filter applied to the entities in location services. There are also service specific paramters that can be used in creating a filter.
Trackers are created by a subscribe request from an application. The subscribe method takes a filter as an input and returns a tracker object. The applicaton can call the trackers query method to obtain a list of entities that match the filter. This list of entities will allso contain service specific data.

Each tracker has a lifetime defined by the location services server. When the lifetime of the tracker is reached, it is deleted. Before the expiration of the tracker, the signal EndOfLifeThreasholdReached will be generated on the tracker informing all applications observing the tracker that it will expire. The argument to the signal is the number of seconds left until the tracker expires. An application can call the KeepAlive method on the tracker to reset the expiration clock.

The tracker contains a Filter property. This is the filter that was used to create the tracker. Each of the location services may augment the base filter with service specific data.

The tracker interface contains a matches method which returns a list of entities that match the filter

The tracker contains a EntityChanged signal. This signal will be delivered to the application every time a change in an entity matches the filter. The signal delivers a single entity. If multiple entities change, multiple signals are delivered.


#### Presence
The presence service announces the entry or exit of an entity to the proximal network.
The presence service can be queried to determine if an entity is or is not present.
Users of the presence service can subscribe to changes in presence of known, anonymous, groups, or all entities.
An entity is deemed not to be present when all of the contributing devices declare it is not present.
An entity is deemed to be present if at least one contributing device claims the entity is present.
The presence service is different from the About service in that entities may not be running Alljoyn services (e.g. beacons...).
##### Presence Entity
A presence entity is an entity and an additional boolean that represents the presence (true) or abscense (false) of the entity.
##### Presence filter
A presence filter contains an entity parser and an entity list.
##### Presence tracker
In the presence tracker, the Matches method returns a list of Presence entities and the EntityChanged signal is delivered with a presence entity.
##### Presence contributer
A presence contributer calls the location services UpdatePresenceInfo methods. This methods takes the contributing devices entity information and a Presence entity of the sensed device.
#### Distance
The distance service provides data on the relative distance between two entities.
Users of this service can query for the distance between a reference entity and a set of entities specified by a distance filter.
Users of this service can subscribe to notifications when an entity comes within a specified distance of another entity, or any member of a group.
or get a notification if an entity moves a specified distance in regards to another entity.
Distances between entities may not be available for all entities in the Distance service. If entity A and entity B are 4 meters apart and entity B and entity C are 2 meters apart, it is not possible to know the distance between entity C and enity A. The distance service does not calculate the distance between entities, it relies on the distance contributing device to provide all distance information.

##### Distance entity
A distance entity is an entity and an addtional integer that represents the distance of this entity from the reference entity.
##### Distance filter
The distance filter contains a reference entity and a location filter. 
##### Distance subscription
The subscribe method
##### Distance tracker
The Distance tracker contains a Filter property. This is the distance filter that was used to create this tracker.

##### Distance contributer
The distance contributer is responsible for sending distance information to the location services. The calculation of that distance and the
#### Geofence
The Geofence service provides information about entities that are en tering or leaving a requested geofence. Users or this service can query for all entities within a geofence of a reference entity that mactches a geofence filter. 
##### Geofence entity
##### Geofence filter
##### Geofence Subscription
##### Geofence tracker
##### Geofence contributer

#### Position
The position service provides two  types of position data. The absolute position of an entity is described by WGS 84 data.  The position service can alternatively specify the position of an entity with respect to an origin is described by origin, X, Y and Z coordinates specified in centimeters. The origin is a string which has a locally determined meaning. Users of this service can query for the position of an entity or a group of entities. Users of this service can also subscribe for position information of an entity or a group of entities and receive signals about change in their position.
Positions sensors (contributing devices) may report either absolute or relative position information.
The horizontal and vertical accuracy of the position data is also returned. This is accuracy information is supplied by the contributing device.

#### Containment
The containment services provides information about which entities reside within a container and which container and entity is within. The details are left for a future release.

### Security
Location service interfaces are secured by Security 2.0.

### Privacy
The service provides the ability to apply a privacy policy to an entity.
It is not possible to provide privacy services to anonymous entities.
The only way to prevent anonymous entities from being reporting  is to stop radio transmission.

Privacy concerns include the ability for an application to control the degree to which an entity is reported to applications.
A location service ACL allows or disallows the location service from reporting on an entity. The default ACL is to allow.
The privacy policy consists of 4 ACLs. Allow/Disallow presence, allow/disallow proximity, allow/disallow location and allow/disallow containment.
A policy can be applied to an entity or a group.

An entities or groups policy may have a white list and/or a black list for each of the services. The white/black lists contain the identity of a consuming applications (Need to figure out what this is).
An entity has a default ACL that can be allow, disallow or callback.  the application that registered the entity into location services.
A group has a default ACL that is allow or disallow.

If a privacy policy is applied to an entity, it will override any policy applied to a group that the entity is a member of.


##### Presence ACL.
The presence privacy policy will determine whether the presence of an entity can be announced.

##### Distance ACL 
The distance ACL is applied to an entity and another entity or group, it determines whether the proximity of the entity can be announced.

#### Geofence ACL
The proximity ACL is applied to an entity and another entity or group, it determines whether the proximity of the entity can be announced.

#### Position ACL
The position privacy policy will determine whether the position of an entity can be announced.

#### Containment ACL
deferred

### Scenarios


### Groups
A group is a dictionary entry that maps a group name to a list of entities.
Groups allow code to be written without knowing the identity of all of the entities. Applications, active entities and contributing devices can add or remove entities from a group.
Groups allow easy management of large numbers of entities.
Groups allow applications to add entities to a group for use by other applications.
A privacy policy can be applied to a group.
Groups are a property of the location services interface.
A property changed signal is generated whenever group membership changes.

