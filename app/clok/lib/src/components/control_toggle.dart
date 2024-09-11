import 'dart:typed_data';

import 'control_base.dart';

class ToggleControl extends BaseControl<bool> {
  ToggleControl(super.optionName, super.optionValue, {super.display = true, super.notifiable = true, super.writeonly = false });
  
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
