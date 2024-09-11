import 'dart:convert';
import 'dart:typed_data';

import '../consts.dart';
import 'control_base.dart';

class StringControl extends BaseControl<String> {
  StringControl(super.optionName, super.optionValue, {super.display = true, super.notifiable = true, super.writeonly = false });
  
  @override
  String decode(Uint8List data) {
    return UTF8_DECODE.convert(data);
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
