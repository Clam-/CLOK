import 'dart:typed_data';

import 'control_base.dart';

class ToggleControl extends BaseControl<bool> {
  ToggleControl(optionName, optionValue, {display = true, notifiable = true}) : 
    super(optionName, optionValue, display: display, notifiable: notifiable);
  
  @override
  bool decode(Uint8List data) {
    return data[0] == 1;
  }
  @override
  Uint8List encode(bool data) {
    return Uint8List.fromList([ data ? 0x01 : 0x00 ]);
  }

  @override
  bool convertType(String value) {
    return bool.parse(value);
  }
}
