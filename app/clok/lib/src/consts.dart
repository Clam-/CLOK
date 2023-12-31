// ignore_for_file: constant_identifier_names, non_constant_identifier_names

import 'dart:convert';

const ASCII_DECODE = AsciiDecoder(allowInvalid: true);
const UTF8_DECODE = Utf8Decoder(allowMalformed: true);

const int MTU_SIZE_REQUEST = 270;
String SERVICE_ID = "00000000-5AAD-BAAD-FFFF-5AD5ADBADC1C".toLowerCase();
String SUFFIX_ID = "-5AAD-BAAD-FFFF-5AD5ADBADC1C".toLowerCase();

// Characteristic consts
String WIFI_SSIDS = "00000011$SUFFIX_ID";
String WIFI_SSID = "00000012$SUFFIX_ID";
String WIFI_WPAKEY = "00000013$SUFFIX_ID";
String WIFI_DOSCAN = "00000014$SUFFIX_ID";
String ROOTCA_URL = "00000031$SUFFIX_ID";
String TZ_ZONEINFO_URL = "00000051$SUFFIX_ID";
String TZ_TIMEZONE = "00000052$SUFFIX_ID";
String TZ_REGIONS = "00000053$SUFFIX_ID";
String TZ_REGION = "00000054$SUFFIX_ID";
String TZ_TIMEZONES = "00000055$SUFFIX_ID";
String TZ_NTP1 = "00000056$SUFFIX_ID";
String TZ_NTP2 = "00000057$SUFFIX_ID";

// alarm stuff TBD

