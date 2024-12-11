import 'dart:typed_data';
import 'control_base.dart';

class StatusControl extends BaseControl<int> {
  StatusControl(super.updatemethod, super.chara, super.optionName, super.optionValue, 
    {super.display = true, super.notifiable = true, super.writeonly = false }
  );
  
  @override
  String toString() {
    if (optionValue < 0 ) { return "Not connected."; }
    else if (optionValue == 0) { return "Connected."; }
    else if (optionValue == 1) { return "Disconnected."; }
    else if (optionValue > 1) { return "Disconnected multiple times. WiFi Key may be wrong. Delete and re-add WiFi."; }
    return "";
  }
  @override
  int decode(ByteData data) {
    return data.getInt8(data.offsetInBytes);
  }
  
  @override
  Uint8List encode(int data) {
    return Uint8List.fromList([data]);
  }

  @override
  int convertType(String value) {
    return int.parse(value);
  }
}