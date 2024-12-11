import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';

import 'control_string.dart';

// "regions" is the main characteristic
// "region" is the chara we write to when picking a region
// "timezones" is the secondary characteristic we listen to for the tzs of the region
// "timezone" is the chara we write to when picking a tz from the above.
class TZSelectControl extends StringControl {
  final BluetoothCharacteristic regionBC;
  final BluetoothCharacteristic timezonesBC;
  final BluetoothCharacteristic timezoneBC;
  String tzString = "";
  final List<DropdownMenuEntry<String>> regions = <DropdownMenuEntry<String>>[];
  final List<DropdownMenuEntry<String>> locations = <DropdownMenuEntry<String>>[];

  TZSelectControl(super.updatemethod, super.chara, super.optionName, super.optionValue,
    { super.display = true, super.notifiable = true, super.writeonly = false,
      required Map<String, BluetoothCharacteristic> extrachars }
  ) : regionBC = extrachars["rc"]!, timezonesBC = extrachars["tsc"]!, timezoneBC = extrachars["tzc"]!
  {
    timezonesBC.value.listen((data) { 
      final d = decode(data);
      locations.add(DropdownMenuEntry(value: d, label: d));
    });
    timezoneBC.value.listen((data) { 
      tzString = decode(data); 
      updatemethod(() {  });
    });
  }

  @override
  String toString() {
    return tzString;
  }

  @override
  Future<void> init() async {
    if (hasInit) { return; }
    await timezoneBC.readValue(timeout: const Duration(seconds: 2));
    hasInit = true;
  }

  @override
  void setValue(ByteData data) { 
    final d = decode(data);
    print("Received value: $d to ${chara.uuid}");
    // blindly add. maybe add check if it exists later?
    regions.add(DropdownMenuEntry<String>(label: d, value: d));
    regions.sort();
  }

  @override
  String convertType(String value) {
    return value;
  }
  @override
  void sendData(String value) { }
  void sendRegion(String value) {
    regionBC.writeValueWithResponse(encode(value));
  }
  void sendLocation(String value) {
    timezoneBC.writeValueWithResponse(encode(value));
  }
  
  @override
  void Function() onTapGen(BuildContext context) {
    // dialog here...
    return () {
      showDialog<String>(
        context: context,
        builder: (BuildContext context) {
          String? regionSelection;
          String? locationSelection;
          return AlertDialog(
            title: Text("$optionName - Select region and location"),
            content: SingleChildScrollView(
              child: Column(
                children: [
                  DropdownMenu<String>(
                    initialSelection: regionSelection,
                    label: const Text('Region'),
                    dropdownMenuEntries: regions,
                    onSelected: (String? twi) {
                      if (twi != null && twi.isNotEmpty) {
                        regionSelection = twi;
                        sendRegion(twi);
                        locations.clear();
                      }
                    },
                  ),
                  DropdownMenu<String>(
                    initialSelection: locationSelection,
                    label: const Text('Location'),
                    dropdownMenuEntries: locations,
                    onSelected: (String? twi) {
                      locationSelection = twi;
                    },
                  ),  
                ]
              ),
            ),
            actions: <Widget>[
              TextButton (
                child: const Text('Cancel'),
                onPressed: () {
                  Navigator.pop(context);
                },
              ),
              FilledButton (
                child: const Text('Save'),
                onPressed: () { 
                  if (locationSelection != null && locationSelection!.isNotEmpty) { sendLocation(locationSelection!); }
                  // send value to characteristic...
                  Navigator.pop(context);
                },
              ),
            ],
          );
        }
      );
    };
  }
}
