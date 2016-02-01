Distance.MD

# org.alljoyn.locationservices.Distance.Distance version 1
## Specification
This interface contains methods for applications to access the data of the distance service
The distance service provides data on the relative distance between two entities.
Users of this service can query for the distance between a reference entity and a set of entities specified by a distance filter.
Users of this service can subscribe to notifications when an entity comes within a specified distance of another entity, or any member of a group.
or get a notification if an entity moves a specified distance in regards to another entity.
Distances between entities may not be available for all entities in the Distance service. 
If entity A and entity B are 4 meters apart and entity B and entity C are 2 meters apart, it is not possible to know the distance between entity C and enity A. 
The distance service does not calculate the distance between entities, it relies on the distance contributing device to provide all distance information.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### DistanceSubscribe(filter) -> (distanceTracker)

|                       |                                             |
|-----------------------|---------------------------------------------|


Subscribe for events that occur when firstEntity and secondEntity are in distance.
Events are only sent again if the distance change is  more than deltaDistance (measured in cm)
This method returns a tracker. 
If a tracker exists that matches the filter it is returned,
otherwise a new tracker object is created.
Trackers have a lifetime and will be reaped after the lifetime is exceeded.

Input arguments:

  * **distanceFilter** --- DistanceFilter --- A filter to match entities
  * **distanceDelta** --- double --- Geofence change distance threshold (measured in cm)
  

Output arguments:

  * **distanceTracker** --- DistanceTracker --- a DistanceTracker object that is on the AllJoyn Bus.

Errors raised by this method:

#### QueryDistance(filter) -> (DistanceEntity)
|                       |                                             |
|-----------------------|---------------------------------------------|

Query for distance information based on the supplied filter

Input Arguments:

* **distanceFilter** --- DistanceFilter --- The filter to be used to select the entities to return 

Output Arguments:

* **distanceEntites** --- a[DistanceEntity] --- An array of distance entities that match the distanceFilter


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
 
#### struct DistanceEntity

Distance services representation of device information
It contains an entity (a human readable description and a unique id of the device), a distance (an integer
representing the distance of the device from a reference entity and a multiplier), and a representation of accuracy (an integer
representing the accuracy of the measurement and a multiplier)

  * **entity** -- Entity -- The entity for the device
  * **distance** --- double --- integer representing the distance of the device from a reference entity  
  * **distanceMultiplier** --- uint32 --- distance multiplier  
  * **accuracy** --- uint32 --- integer representing the accuracy of the measurment  
  * **accuracyMultiplier** --- uint32 --- accuracy multiplier


#### struct DistanceFilter

Service settings along with selection criteria for specifying entities of interest
The filter contains a regular expression to match the field to match with entities in the distance service.

  * **referenceEntity** -- Entity -- The reference entity is the entity from which distances to other entites are measured
  * **entityParser** --- Entity --- The entity parser is an entity filled with regular expressions for matching
  * **entityList** --- a[Entity] --- An array of entities that match the filter criteria


## References

  * The xml definition of the [Distance service] (Distance-v1.xml)
  * The [Theory of Operations] (theory-of-operations)
  * The [DistanceTracker] (DistanceTracker-v1)
  * the [DistanceContributor] (DistanceContributor)
