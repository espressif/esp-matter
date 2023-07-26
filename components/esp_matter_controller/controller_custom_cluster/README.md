# Matter Controller Cluster

The Matter Controller Cluster offers an interface for managing the ESP Matter Controller. It allows users to perform various tasks, such as authorizing the controller to login to the cloud, updating the controller's NOC to the Rainmaker-Fabric User NOC, and notifying the controller to update the device list.

## 1. Cluster Identifiers

| Identifier | Name                  |
|------------|-----------------------|
| 0x131BFC01 | **Matter Controller** |

## 2. Attributes

| ID     | Name                    | Type           | Constranint | Quality | Default | Access | Conformance |
|--------|-------------------------|----------------|-------------|---------|---------|--------|-------------|
| 0x0000 | **RefreshToken**        | string         |             | N       |         | R V    | M           |
| 0x0001 | **AccessToken**         | string         |             | N       |         | R V    | M           |
| 0x0002 | **Authorized**          | bool           |             | N       | false   | R V    | M           |
| 0x0003 | **UserNOCInstalled**    | bool           |             | N       | false   | R V    | M           |
| 0x0004 | **EndpointURL**         | string         |             | N       |         | R V    | M           |
| 0x0005 | **RainmakerGroupId**    | string         |             | N       |         | R V    | M           |
| 0x0006 | **UserNOCFabricIndex**  | uint8_t        |             | N       |         | R V    | M           |

### 2.1 RefreshToken Attribute

This attribute stores the refresh token. For the HTTP REST Authenticated APIs, the refresh_token SHALL be used to fetch the access_token and the access_token will be passed in the "Authorization" HTTP header as the authentication token.

Note: The access_token validity will expire after 1 hour.

### 2.2 AccessToken Attribute

This attribute stores the access token. It will be updated after the controller is authorized successfully. For the HTTP REST Authenticated APIs, the access_token will be passed in "Authorization" HTTP header as the authentication token. If the HTTP REST Authenticated APIs fail with decription "Unauthorized", the access_token will be updated.

### 2.3 Authorized Attribute

This attribute indicates the authorization status of the controller after joining the Rainmaker Fabric. After the access_token is fetched, the Authorized value will be set to true. And it will be set to false after 1 hour. If the HTTP REST Authenticated APIs fail with decription "Unauthorized", this attribute will be also set to false.

### 2.4 UserNOCInstalled Attribute

This attribute indicates whether the User NOC is installed in the controller. After the controller is authorized, the controller will generate a CSR and send it with the RainmakerGroupId to the cloud. The response from the cloud will include the new User NOC which will be installed in the controller.

### 2.5 EndpointURL Attribute

This attribute stores the Endpoint URL that is used for HTTP REST APIs.

### 2.6 RainmakerGroupId Attribute

This attribute stores the Rainmaker Group Id which is bound to the Matter Fabric Id.

### 2.7 UserNOCFabricIndex Attribute

This attribute stores the fabric index of the fabric where the user NOC is installed. It will be updated after the user NOC is installed.

## 3. Commands

| ID     | Name                     | Direction      | Response | Access | Conformance |
|--------|--------------------------|----------------|----------|--------|-------------|
| 0x0000 | **AppendRefreshToken**   | client->server | Y        | A      | M           |
| 0x0001 | **ResetRefreshToken**    | client->server | Y        | A      | M           |
| 0x0002 | **Authorize**            | client->server | Y        | A      | M           |
| 0x0003 | **UpdateUserNOC**        | client->server | Y        | A      | M           |
| 0x0004 | **UpdateDeviceList**     | client->server | Y        | A      | M           |


### 3.1 AppendRefreshToken Command

The AppendRefreshToken command allows the controller to write the RefreshToken Attribute in several commands. It will append the current RefreshToken attribute. The RefreshToken is about 1700 Bytes which is much longer than the UDP MTU(1280 Bytes) so it should be send it separately.

The AppendRefreshToken command SHALL have the following data fields:

| ID | Name                     | Type   | Constraint | Quality | Default | Comformance |
|----|--------------------------|--------|------------|---------|---------|-------------|
| 0  | **AppendedRefreshToken** | string |  max 1024  |         |         | M           |

#### 3.1.1 AppendedRefreshToken Field

This field is the string which will be appended to the current RefreshToken Attribute.

### 3.2 ResetRefreshToken Command

The ResetRefreshToken command allows devices to reset it RefreshToken to an empty string.

The ResetRefreshToken has no data field.

### 3.3 Authorize Command

The Authorize command allows the controller to login the cloud HTTP server and fetch the AccessToken with the HTTP REST Authenticated APIs.

After AccessToken is fetched, the controller will get the RainmakerGroupId according to the FabricId of the command object.

The Authorize command SHALL have the following data fields:

| ID | Name             | Type   | Constraint | Quality | Default | Comformance |
|----|------------------|--------|------------|---------|---------|-------------|
| 0  | **EndpointURL**  | string |  max 64    |         |         | M           |

#### 3.3.1 EndpointURL Field

This field is the EndpointURL. After the controller is authorized successfully, this value will be written to the EndpointURL Attribute.

### 3.4 UpdateUserNOC Command

The UpdateUserNOC command allows the controller to fetch the Rainmaker User NOC after the is authorized. When receiving this command, the controller will generate a new CSR and send it with the RainmakerGroupId to the cloud. After receiving the response, it will install the RainmakerUserNOC in the response.

The UpdateUserNOC command has no data field.

### 3.5 UpdateDeviceList Command

The UpdateDeviceList command notifies the controller to update its maintaining Matter device list. After this command is received, the controller will query the device list in the Rainmaker Group(Matter Fabric) from the cloud. This command SHALL be called after the controller has updated its NOC and when a new Matter device is commissioned into the Matter Fabric.

The UpdateDeviceList command has no data field.
