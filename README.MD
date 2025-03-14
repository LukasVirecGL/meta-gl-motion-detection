# RDK-B Meta layer | Wi-Fi Motion Detection

This meta layer provides the implementation of the Wi-Fi Sensing solution based on Kurtosis algorithm for RDK-B platform.
This solution was tested on Banana Pi BPI-R4 with Mediatek WiFi 7 card (MT7996).

The application parses CSI data from a connected 5 GHz Wi-Fi client and provides data via TR-181 data model.

## Use cases
**Home security** - privacy-respecting monitoring. Detection of unexpected motion, alert homeowners to any unusual activity.

**Energy efficiency** - optimization of energy usage in homes and buildings by detecting when rooms are occupied and adjusting lighting, heating, and cooling systems accordingly.

**Home Automation** - automation based on presence and movement. Input for other IoT devices.

## High-level design
Solution consists of the following components:

1. **CcspMotionDetection** - TR-181 configuration and management of the WiFi sensing and motion detection
2. **hal-motiondetector** - implementation of motion detector - reading data from WiFi driver, parsing CSI data and motion estimation
3. **Android application** - presentation of motion data and motion events. Enables to select WiFi client for monitoring.

# Data Model (TR-181)

| Name | Type | Default | Write | Description |
| --- | --- | --- | --- | --- |
| **Device.WiFi.X_RDK_MotionDetection.** | object | | R | Parent object of WiFi Sensing application |
| Enable | boolean | true | W | Enables motion detection. If False - CSI data collection for all devices will be stopped.|
| Status | string |  | R | DISABLED - motion detection is not running IDLE - motion detector is enabled but no active devices ACTIVE -  motion detection is running and is active for at least 1 device |
| Motion | unsignedInt | | R | Current motion value in percentage |
| SensingThresholdUp | unsignedInt | 60 | W | Threshold in percentage which will create event MOTION_STARTED when it's exceeded. |
| SensingThresholdDown | unsignedInt | 30 | W | Threshold in percentage which will create event MOTION_STOPEED when it falls under this value. |
| SensingDeviceMACAddress | string | | W | Selection of the device which will be used for Motion detection.  MAC address of the STA transmitting the frame on which CSI data will be gathered. Setting an empty string will stop Motion detection. |
| MonitorInterval | unsignedInt | 1000 | W | TIme in milliseconds which specifies how often CSI data should be collected |
| SensingEventsNumberOfEntries | unsignedInt |  | R | The number of events |
| **Device.WiFi.X_RDK_MotionDetection.SensingEvents.{i}.** | object | | R | Motion sensing events |
| SensingDevice | string | | R | Corresponding MAC addresses of the devices where motion was detected |
| Type | string | | R | MOTION_STARTED - motion value exceeded SensingThresholdUp MOTION_STOPPED - motion value fell under SensingThresholdDown |
| Time | string | | R | Time when motion event was detected	|
