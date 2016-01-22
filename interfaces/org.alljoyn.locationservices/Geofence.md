Geofence.MD

# org.alljoyn.locationservices.Distance.Geofence version 1
## Specification
The Geofence service provides information about entities that are entering or leaving a requested geofence. 
Users or this service can query for all entities within a geofence of a reference entity that mactches a geofence filter. 
The Geofence interface is used by applications to retrieve geofence event data from the geofence service.
An application can query the service using a GeofenceFilter and recieve zero or more entities that
match the filter. Alternativly an application can subscribe to the geofence service using a filter. 
The service will return a GeofenceTracker object.
The service will signal the application using the GeofenceTracker object when the presence of an 
entity that matches the filter changes.

 

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### GeofenceGeofenceSubscribe(filter) -> (geofenceTracker)

|                       |                                             |
|-----------------------|---------------------------------------------|


Subscribe for events that occur when firstEntity and secondEntity are in geofence.
Events are only sent again if the geofence change is more than deltaGeofence (measured in cm)
This method returns a tracker. 
If a tracker exists that matches the filter it is returned,
otherwise a new tracker object is created.
Trackers have a lifetime and will be reaped after the lifetime is exceeded.

Input arguments:

  * **filter** --- GeofenceFilter --- A filter to match entities
  * **geofenceDelta** --- uint32 --- Geofence change distance threshold (measured in cm)
  

Output arguments:

  * **geofenceTracker** --- GeofenceTracker --- a GeofenceTracker object that is on the AllJoyn Bus.

Errors raised by this method:

#### QueryGeofence(filter) -> (GeofenceEntity)
|                       |                                             |
|-----------------------|---------------------------------------------|

Query the service for all entities that match the filter (Needs Updating)

Input Arguments:

* **filter** --- GeofenceFilter --- The filter to be used to select the geofence entities to return 

Output Arguments:

* **geofenceEntities** --- GeofenceEntity[] --- An array of geofence entities that match the geofenceFilter

#### QueryAllGeofenceGeofence(entity) -> (GeofenceEntity)

|                       |                                             |
|-----------------------|---------------------------------------------|

This method is a convenience method, it creates a filter for the passed entity and calls QueryGeofence

Input Arguments

* **entity** --- Entity --- An Entity
* **geofence** --- uint32 --- (Needs Updating)
* **geofenceMultiplier** --- uint32 --- (Needs Updating)


Output Arguments:

* **nearbyEntities** --- a[GeofenceEntity] --- Returns an array of Geofence Entity objects

#### struct Entity

Entities are objects that are tracked by the location services.
An entity may or may not be a member of the AllJoyn bus. 
An entity may be known or anonymous. 
A known entity is registered with the service by an application. 
Entities that are discovered, and not pre-registered are designated as unknown entities.
An entity is comprised of two strings, a unique identifier and a media specific identifier. 
The unique identifier is a human readable string.
The media specific identifier will normally be the MAC address. 
Location Services uses both the unique identifier and the media specific identifier when matching 
entities. Anonymous entities will have a unique identifier of 00000000-0000-0000-0000-000000000000
and a media specific identifier of their MAC address.

  * **entityId** --- string --- A unique ID associated with an entity (usually a UUID)
  * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)
 
#### struct GeofenceEntity

A GeofenceEntity is a representation of a device in the geofence service. 
It contains an entity (a human readable description and a unique id of the device) and an integer
representing ... 

  * **entity** -- Entity -- The entity for the device
  * **geofence** ---uint32--- (Needs Updating).

#### struct GeofenceFilter

A GeofenceFilter is used to match entities the application is interested in.
The filter contains a regular expression to match ain the field to match with entities in the location services. (Needs Updating)

  * **entity** -- Entity -- The entity for the device
  * **filter** --- Entity --- A filter is an entity with regular expressions in the field to match with entities in the location services
  * **entities** --- Entity array --- An array of Entities that match the filter criteria


## References

  * The xml definition of the [Geofence service] (Geofence-v1.xml)
  * The [Theory of Operations] (theory-of-operations)
  * The [GeofenceTracker] (GeofenceTracker-v1)
  * the [GeofenceContributor] (GeofenceContributor)
