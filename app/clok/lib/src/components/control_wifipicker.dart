import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';

import '../consts.dart';
import 'control_base.dart';

class WiFiItem implements Comparable<WiFiItem> {
  final int rssi;
  final String ssid;
  WiFiItem(this.rssi, this.ssid);
  @override
  int compareTo(WiFiItem other) { return other.rssi - rssi; }
}

class WiFiPickerControl extends BaseControl<String> {
  final TextEditingController wifiController = TextEditingController();
  final List<DropdownMenuEntry<WiFiItem>> wifiEntries = <DropdownMenuEntry<WiFiItem>>[];
  static Comparator<DropdownMenuEntry<WiFiItem>> nameComparator = (a, b) => a.value.compareTo(b.value);
  WiFiItem? wi;

  WiFiPickerControl(super.optionName, super.optionValue, {super.display = true, super.notifiable = true, super.writeonly = false });
  
  @override
  String decode(Uint8List data) {
    return UTF8_DECODE.convert(data);
  }
  @override
  Uint8List encode(String data) {
    return Uint8List.fromList(utf8.encode(data));
  }
  @override
  void setValue(Uint8List data) { 
    final d = decode(data);
    final di = d.split("|");
    final rssi = int.tryParse(di[0]);
    if (rssi == null) { return; }
    final ssid = di.sublist(1).join("|");
    var found = false;
    for(final i in wifiEntries) {
      if (i.value.ssid == ssid) { found = true; break; }
    }
    if (!found) { wifiEntries.add(DropdownMenuEntry<WiFiItem>(value:WiFiItem(rssi, ssid), label: ssid)); wifiEntries.sort(nameComparator); }
  }

  @override
  String convertType(String value) {
    return value;
  }
  @override
  void sendData(String value) {
    // ignore value because this is special...

  }
  
  @override
  void Function() onTapGen(BuildContext context) {
    // dialog here...
    return () {
      showDialog<String>(
        context: context,
        builder: (BuildContext context) {
          TextEditingController textFieldController = TextEditingController(text: optionValue.toString());
          
          return AlertDialog(
            title: Text(optionName),
            content: SingleChildScrollView(
              child: Column(
                children: [
                  DropdownMenu<WiFiItem>(
                    initialSelection: wi,
                    controller: wifiController,
                    label: const Text('SSID'),
                    dropdownMenuEntries: wifiEntries,
                    onSelected: (WiFiItem? twi) {
                      wi = twi;
                    },
                  ),
                  TextField(
                    controller: textFieldController,
                    decoration: const InputDecoration(hintText: "Key"),
                  )    
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
                  sendData("");
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
