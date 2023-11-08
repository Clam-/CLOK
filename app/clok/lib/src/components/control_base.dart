import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:quick_blue/quick_blue.dart';

import '../type_converters/base_convert.dart';

abstract class BaseControl<V> implements BaseConverter<V> {
  late String deviceID;
  late String serviceID;
  late String characteristicID;
  final String optionName;
  V optionValue;
  late int mtu;
  final bool display;
  final bool notifiable;
  
  BaseControl(this.optionName, this.optionValue, {this.display = true, this.notifiable = true });

  void setDeviceOpts(String did, int m) { deviceID = did; mtu = m;}
  void setServiceID(String sid) { serviceID = sid; }

  void setValue(Uint8List data) { 
    print("SetValue: $data");
    optionValue = decode(data);
    print("Stored decoded value: $optionValue");
  }

  void sendData(V value) {
    print("Sending $value");
    QuickBlue.writeValue(deviceID, serviceID, characteristicID, Uint8List.fromList(encode(value)), BleOutputProperty.withResponse);
  }

  void Function() onTapGen(BuildContext context) {
    // dialog here...
    return () {
      showDialog<String>(
        context: context,
        builder: (BuildContext context) {
          TextEditingController textFieldController = TextEditingController(text: optionValue.toString());
          return AlertDialog(
            title: Text(optionName),
            content: TextField(
              controller: textFieldController,
              decoration: const InputDecoration(hintText: "Value"),
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
                  sendData(convertType(textFieldController.text));
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
