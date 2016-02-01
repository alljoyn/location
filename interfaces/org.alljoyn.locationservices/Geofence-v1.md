Geofence.MD

# org.alljoyn.locationservices.Distance.Geofence version 1
## Specification
The Geofence service provides information about entities that are entering or leaving a requested geofence. 
Users or this service can query for all entities within a geofence of a reference entity that mactches a geofence filter. 
The Geofence interface is used by applications to retrieve geofence event data from the geofence service.
An application can query the service using a GeofenceFilter and recieve zero or more entities that
match the filter. Alternativly an application can subscribe to the geofence service using a filter. 
The service will return a GeofenceTracker object.
The service will signal the application using the GeofenceTracker object when the entity that matches the filter enters or leaves the geofence.

 

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### GeofenceSubscribe(filter) -> (geofenceTracker)

|                       |                                             |
|-----------------------|---------------------------------------------|


Subscribe for events that occur when matching entities enter or leave the geofence.
Events are sent only once if an entity enters or leaves the geofence.
This method returns a tracker. 
If a tracker exists that matches the filter it is returned,
otherwise a new tracker object is created.
Trackers have a lifetime and will be reaped after the lifetime is exceeded.

Input arguments:

  * **geofenceFilter** --- GeofenceFilter --- A filter to match entities
  

Output arguments:

  * **geofenceTracker** --- GeofenceTracker --- a GeofenceTracker object that is on the AllJoyn Bus.

Errors raised by this method:

#### QueryGeofence(filter) -> (GeofenceEntity)
|                       |                                             |
|-----------------------|---------------------------------------------|

Query the service for all entities that match the filter. Entities both inside and outside the geofence are returned.
Input Arguments:

* **geofenceFilter** --- GeofenceFilter --- The filter to be used to select the geofence entities to return 

Output Arguments:

* **geofenceEntities** --- GeofenceEntity[] --- An array of geofence entities that match the geofenceFilter

#### struct Entity

Entity is a generic representation of a device
Entities are objects that are tracked by the location services.
An entity may or may not be a member of the AllJoyn bus. 
An entity may be known or anonymous. 
A known entity is registered with the service by an application. 
Entities that are discovered, and not pre-registered are designated as unknown entities.
An entity is comprised of two strings, a unique identifier and a media specific identifier. 
The unique identifier is a human readable string.
The media specific identifier is a unique descriptive string or it can be the MAC address if available. 
Location Services uses both the unique identifier and the media specific identifier when matching 
entities. Anonymous entities will have a unique identifier of 00000000-0000-0000-0000-000000000000
and a media specific identifier.

  * **entityDescriptor** --- string --- A human readable description of the entity
  * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)
 
#### struct GeofenceEntity

A GeofenceEntity is Geofence services representation of device information 
It contains an entity (a human readable description and a unique id of the device) and an integer
representing ... 

  * **entity** -- Entity -- The entity for the device
  * **withinFenceline** --- boolean --- boolean to indicate if the device is within the fenceline

#### struct GeofenceFilter

A GeofenceFilter is used to match Service settings along with selection criteria for specifying entities of interest

  * **referenceFenceline** -- double -- The reference fenceline is the distance around the reference entity that forms the geofence boundary
  * **referenceEntity** -- Entity -- The reference entity is the entity from which distances to other entites are measured
  * **entityParser** --- Entity --- The entity parser is an entity filled with regular expressions for matching
  * **entityList** --- a[Entity] --- The entity list is a list of entities for matching

## References

  * The xml definition of the [Geofence service] (Geofence-v1.xml)
  * The [Theory of Operations] (theory-of-operations)
  * The [GeofenceTracker] (GeofenceTracker-v1)
  * the [GeofenceContributor] (GeofenceContributor)
