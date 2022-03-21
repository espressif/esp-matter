# 2021-12-07 (esp_matter: New data model)

This commit creates the dynamic ESP Matter data model and uses that instead of the
static data model in zap-generated.

The examples create the ESP Matter data model using the new APIs and default configs.
These APIs add the mandatory clusters and the corresponding attributes and commands for the
device type (endpoint) created.
```
esp_matter_node_config_t node_config = NODE_CONFIG_DEFAULT();
esp_matter_node_t *node = esp_matter_node_create(&node_config, app_attribute_update_cb, NULL);

esp_matter_endpoint_color_dimmable_light_config_t light_config = ENDPOINT_CONFIG_COLOR_DIMMABLE_LIGHT_DEFAULT();
esp_matter_endpoint_t *endpoint = esp_matter_endpoint_create_color_dimmable_light(node, &light_config, ESP_MATTER_ENDPOINT_FLAG_NONE);
```

The examples have also been restructured and the matter submodule specific initialisations have
been moved to the esp_matter component and are called from the application with the new API.
```
typedef void (*esp_matter_event_callback_t)(const ChipDeviceEvent *event, intptr_t arg);

esp_err_t esp_matter_start(esp_matter_event_callback_t callback);
```

There is now just one attribute update callback which calls the other callbacks in the application.
The application receives this callback twice, once before updating the value in the data model (pre_attribute)
and once after updating the value (post_attribute).
```
esp_err_t app_attribute_update_cb(esp_matter_callback_type_t type, int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val, void *priv_data);
```

The app_driver component has been moved to the application itself and it now uses the endpoint_id,
cluster_id, attribute_id for setting/getting the values. The application calls the the attribute_update
for drivers first when it gets the pre_attribute callback. If this returns success, i.e. ESP_OK, only then
the data model value is updated and the application receives the post_attribute callback.
```
esp_err_t app_driver_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val);
```

The rainmaker example creates its data model dynamically based on the ESP Matter data model. New
params can be easily supported in the rainmaker example by adding to the app_rainmaker_get_* APIs.
The application calls the attribute_update for rainmaker and other ecosystems when it gets the post_attribute
callback.
```
/* Create a device and add the relevant parameters to it */
app_rainmaker_device_create();

esp_err_t app_rainmaker_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val);
```

Non-mandatory endpoints, clusters, attributes or commands can also be easily added by using the
low level APIs. New device types (endpoints) and supporting clusters can be added from the spec
by looking at the existing APIs for reference.
```
esp_matter_node_t *esp_matter_node_create_raw();
esp_matter_endpoint_t *esp_matter_endpoint_create_raw(esp_matter_node_t *node, uint8_t flags);
esp_matter_cluster_t *esp_matter_cluster_create(esp_matter_endpoint_t *endpoint, int cluster_id, uint8_t flags);
esp_matter_attribute_t *esp_matter_attribute_create(esp_matter_cluster_t *cluster, int attribute_id, uint8_t flags, esp_matter_attr_val_t val);
esp_matter_command_t *esp_matter_command_create(esp_matter_cluster_t *cluster, int command_id, uint8_t flags, esp_matter_command_callback_t callback);
```

Another zap_light example has been added for backward compatibility, which uses the static data model
and the default zap-generated.
