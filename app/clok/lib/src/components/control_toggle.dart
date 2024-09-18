import 'dart:typed_data';

import 'control_base.dart';

class ToggleControl extends BaseControl<bool> {
  ToggleControl(super.updatemethod, super.chara, super.optionName, super.optionValue, 
    {super.display = true, super.notifiable = true, super.writeonly = false }
  );
  
  @override
  bool decode(ByteData data) {
    return data.getInt8(data.offsetInBytes) == 1;
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
