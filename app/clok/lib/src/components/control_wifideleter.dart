import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';

import 'control_string.dart';

class WiFiDeleteControl extends StringControl {
  final List<String> wifiEntries = [];
  BluetoothCharacteristic dc;

  WiFiDeleteControl(super.updatemethod, super.chara, super.optionName, super.optionValue, 
    {super.display = true, super.notifiable = true, super.writeonly = false, 
    required Map<String, BluetoothCharacteristic> extrachars }
  ): dc = extrachars["wdc"]!;
  
  @override
  String toString() {
    return wifiEntries.length.toString();
  }

  @override
  void setValue(ByteData data) { 
    final d = decode(data);
    if (d == ":") { return; }
    print("Received value: $d to ${chara.uuid}");
    if (!wifiEntries.contains(d)) {
      wifiEntries.add(d);
    }
    wifiEntries.sort();
  }

  @override
  String convertType(String value) {
    return value;
  }
  
  @override
  void Function() onTapGen(BuildContext context) {
    // dialog here...
    return () {
      showDialog<String>(
        context: context,
        builder: (BuildContext context) {
          String? selected = wifiEntries.isNotEmpty ? wifiEntries[0] : null;
          return AlertDialog(
            title: Text("$optionName - Select to delete"),
            content: 
              Container(
                width: double.maxFinite,
                child: ListView.builder(itemBuilder: (context, index) {
                    return RadioListTile(groupValue: selected,
                      value: wifiEntries[index],
                      title: Text(wifiEntries[index]),
                      onChanged: (value) => selected = value,
                    );
                  },
                  itemCount: wifiEntries.length)
              ),
            actions: <Widget>[
              TextButton (
                child: const Text('Cancel'),
                onPressed: () {
                  Navigator.pop(context);
                },
              ),
              FilledButton (
                child: const Text('DELETE'),
                onPressed: () async {
                  if (selected != null && selected!.isNotEmpty) {
                    await dc.writeValueWithResponse(encode(selected!));
                  }
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
