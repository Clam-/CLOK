import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:quick_blue/quick_blue.dart';

import '../consts.dart';

/// A placeholder class that represents an entity or model.
class SimpleControl {
  SimpleControl(this.optionName, this.optionValue);
  late String deviceId;
  late String characteristicID;
  final String optionName;
  String optionValue;
  late int mtu;

  void setDeviceOpts(String value, int m) { deviceId = value; mtu = m;}

  void setData(String value) { optionValue = value; }

  void sendData(String value) {
    QuickBlue.writeValue(deviceId, SERVICE_ID, characteristicID, Uint8List.fromList(utf8.encode(value)), BleOutputProperty.withoutResponse);
  }

  void Function() onTapGen(BuildContext context) {
    // dialog here...
    
    return () {
      showDialog<String>(
        context: context,
        builder: (BuildContext context) {
          TextEditingController textFieldController = TextEditingController();
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
                  sendData(textFieldController.text);
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
