import 'dart:typed_data';

abstract class BaseConverter<T> {
  T decode(Uint8List data);
  Uint8List encode(T data);
  T convertType(String value);
}