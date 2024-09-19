import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';

import '../consts.dart';
import 'control_base.dart';

class WiFiDeleteControl extends BaseControl<String> {
  final List<String> wifiEntries = [];
  BluetoothCharacteristic? dc;

  WiFiDeleteControl(super.updatemethod, super.chara, super.optionName, super.optionValue, 
    {super.display = true, super.notifiable = true, super.writeonly = false, this.dc }
  );
  
  void addChara(BluetoothCharacteristic c) { dc = c; }

  @override
  String decode(ByteData data) {
    return UTF8_DECODE.convert(data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
  }
  @override
  Uint8List encode(String data) {
    return Uint8List.fromList(utf8.encode(data));
  }
  @override
  void setValue(ByteData data) { 
    final d = decode(data);
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
  void sendData(String value) {
    dc?.writeValueWithResponse(encode(value));
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
                onPressed: () {
                  if (selected != null && selected!.isNotEmpty) { sendData(selected!); }
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
