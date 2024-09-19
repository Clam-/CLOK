import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';

import '../type_converters/base_convert.dart';

abstract class BaseControl<V> implements BaseConverter<V> {
  Function updatemethod;
  BluetoothCharacteristic chara;
  final String optionName;
  V optionValue;
  final bool display;
  final bool notifiable;
  final bool writeonly;
  
  BaseControl(this.updatemethod, this.chara, this.optionName, this.optionValue, {this.display = true, this.notifiable = true, this.writeonly = false }) {
    if (!this.writeonly) {
      //this.getValue();
      if (this.chara.properties.notify) {
        this.chara.value.listen((ByteData d) { setValue(d); updatemethod((){});});
      }
    }
  }
  @override
  bool operator ==(other) => optionName.hashCode == other.hashCode;

  @override
  int get hashCode => optionName.hashCode;

  void setValue(ByteData data) { 
    print("SetValue: $data");
    optionValue = decode(data);
    print("Stored decoded value: $optionValue");
  }

   void getValue() async {
    print("Attemping get: ${chara.uuid}");
    if (!writeonly) { 
      if (this.chara.isNotifying) { this.chara.readValue(); }
      else { this.setValue(await this.chara.readValue()); }
    }
  }

  void sendData(V value) {
    print("Sending $value to ${chara.uuid}");
    this.chara.writeValueWithResponse(encode(value));
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
