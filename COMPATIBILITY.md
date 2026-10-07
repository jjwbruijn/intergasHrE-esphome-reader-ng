# ESPHome 2026.9 compatibility

Target: ESPHome 2026.9.1.

## Fixed incompatibility

The upstream component called:

```python
sens.set_disabled_by_default(...)
```

for sensors, binary sensors and text sensors.

ESPHome 2026.9 moved entity metadata configuration into the entity registration/configuration path. The generated C++ `Sensor`, `BinarySensor` and `TextSensor` classes no longer expose `set_disabled_by_default()`. The setting is still supported and is part of the validated entity configuration.

The patch therefore:

1. Imports `CONF_DISABLED_BY_DEFAULT`.
2. Copies each validated entity config before entity creation.
3. Adds `CONF_DISABLED_BY_DEFAULT` to that config.
4. Lets ESPHome 2026.9's `new_sensor()`, `new_binary_sensor()` and `new_text_sensor()` handle the metadata.

This preserves the component's existing enabled/disabled defaults without generating calls to the removed C++ setter.

## Scope

No unrelated C++ protocol behavior was changed. In particular, the existing state-block handling is left as-is, even though the source defines blocks 0–4 while `STATE_BLOCK_COUNT` is 2. That is a protocol/logic question rather than an ESPHome 2026.9 API compatibility change and should be fixed separately only after confirming the intended Intergas command sequence.
