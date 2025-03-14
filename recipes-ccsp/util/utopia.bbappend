# Default values for the Motion Detection
# Putting them there for syscfg

do_install_append () {
    echo "" >> ${D}${sysconfdir}/utopia/system_defaults
    echo "#====[ Motion Detection ]====#" >> ${D}${sysconfdir}/utopia/system_defaults
    echo "\$MotionDetection_Enable=1" >> ${D}${sysconfdir}/utopia/system_defaults
    echo "\$MotionDetection_SensingThresholdUp=60" >> ${D}${sysconfdir}/utopia/system_defaults
    echo "\$MotionDetection_SensingThresholdDown=30" >> ${D}${sysconfdir}/utopia/system_defaults
    echo "\$MotionDetection_MonitorInterval=1000" >> ${D}${sysconfdir}/utopia/system_defaults
    echo "" >> ${D}${sysconfdir}/utopia/system_defaults
}