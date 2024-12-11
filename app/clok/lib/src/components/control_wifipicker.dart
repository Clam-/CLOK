import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';

import 'control_string.dart';

class WiFiItem implements Comparable<WiFiItem> {
  final int rssi;
  final String ssid;
  WiFiItem(this.rssi, this.ssid);
  @override
  int compareTo(WiFiItem other) { return other.rssi - rssi; }
}

class WiFiPickerControl extends StringControl {
  final List<DropdownMenuEntry<WiFiItem>> wifiEntries = <DropdownMenuEntry<WiFiItem>>[];
  static Comparator<DropdownMenuEntry<WiFiItem>> nameComparator = (a, b) => a.value.compareTo(b.value);
  WiFiItem? wi;
  final BluetoothCharacteristic ssidchar;
  final BluetoothCharacteristic keychar;

  WiFiPickerControl(super.updatemethod, super.chara, super.optionName, super.optionValue,
    {super.display = true, super.notifiable = true, super.writeonly = false, 
    required Map<String, BluetoothCharacteristic> extrachars }
  ) : 
  ssidchar = extrachars["wsc"]!, 
  keychar = extrachars["wkc"]!;

  @override
  String toString() {
    return "Found networks: ${wifiEntries.length}";
  }

  @override
  void setValue(ByteData data) { 
    print("RECEIVED BYTES:");
    print(data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
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
  void sendCustomData(String ssid, String key) async {
    if (ssid.isNotEmpty){
      print("Sending SSID: $ssid, Key: $key");
      print("RAW SSID: ${encode(ssid)}, Raw Key: ${encode(key)}");
      await ssidchar.writeValueWithResponse(encode(ssid));
      await keychar.writeValueWithResponse(encode(key));
    }
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
                  if (wi != null){
                    sendCustomData(wi!.ssid, textFieldController.text);
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
