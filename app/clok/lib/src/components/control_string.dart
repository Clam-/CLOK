import 'dart:convert';
import 'dart:typed_data';

import '../consts.dart';
import 'control_base.dart';

class StringControl extends BaseControl<String> {
  StringControl(super.updatemethod, super.chara, super.optionName, super.optionValue, 
    {super.display = true, super.notifiable = true, super.writeonly = false }
  );
  
  @override
  String decode(ByteData data) {
    print("${data.offsetInBytes} - ${data.lengthInBytes} - ${data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes)}");
    return UTF8_DECODE.convert(data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
  }
  
  @override
  Uint8List encode(String data) {
    return Uint8List.fromList(utf8.encode(data));
  }

  @override
  String convertType(String value) {
    return value;
  }
}