import 'dart:typed_data';

abstract class BaseConverter<T> {
  T decode(ByteData data);
  Uint8List encode(T data);
  T convertType(String value);
}