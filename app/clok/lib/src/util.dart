
String stringToIntToMAC(String s) {
  if (s.contains(":")) { return s; }
  var r = RegExp(r'..');
  var smac = int.parse(s).toRadixString(16).padLeft(12, '0');
  var matches = r.allMatches(smac);
  var mapped = matches.map((e) => e[0]);
  var mr = mapped.toList().reversed.toList();
  var f6 = mr.take(6);
  var f6r = f6.toList().reversed;
  var joined = f6r.join(':');
  return joined;    
}

// oh don't need this since I can specify min/max ahahah...
double processRSSI(int rssi) {
  int shifted = rssi.abs()-20; // shit/offset it a little
  return (shifted/100.0)*-1;
}