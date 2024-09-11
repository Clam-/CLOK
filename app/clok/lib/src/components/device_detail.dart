import 'dart:typed_data';
import 'package:clok/src/components/control_base.dart';
import 'package:convert/convert.dart';
import 'package:quick_blue/quick_blue.dart';
import 'package:flutter/material.dart';

import '../consts.dart';
import '../settings/settings_view.dart';
import 'control_string.dart';
import 'control_toggle.dart';
import 'control_wifipicker.dart';

/// Displays detailed information about a SampleItem.
class DeviceDetailsView extends StatefulWidget {
  const DeviceDetailsView({super.key, required this.deviceId});
  static const routeName = '/device';
  final String deviceId;

@override
  State<DeviceDetailsView> createState() => _DeviceDetailsView();
}

class _DeviceDetailsView extends State<DeviceDetailsView> {

  // state vars
  bool _bleConnected = false;
  List<dynamic Function()> getQueue = [];
  
  // Options List
  final items = <String, BaseControl>{
      // RootCA
      ROOTCA_URL : StringControl("RootCA URL", ""),
      // TZ
      TZ_ZONEINFO_URL : StringControl("TZ ZoneInfo URL", ""),
      //TZListControl("TZ Select Timezone"),
      TZ_NTP1 : StringControl("TZ NTP Server 1", ""),
      TZ_NTP2 : StringControl("TZ NTP Server 2", ""),
      // WiFi AP list
      WIFI_SSIDS : WiFiPickerControl("Add WiFi SSID & Key", ""),
      WIFI_DOSCAN : ToggleControl("WiFi Scan", false, display: false),
  };
  final itemsDevMap = {};

  @override
  void initState() {
    super.initState();
    // setup handlers
    QuickBlue.setConnectionHandler(_handleConnectionChange);
    QuickBlue.setServiceHandler(_handleServiceDiscovery);
    QuickBlue.setValueHandler(_handleValueChange);
    QuickBlue.connect(widget.deviceId);
  }
  void _handleConnectionChange(String devId, BlueConnectionState state) {
    print('_handleConnectionChange $devId, $state');
    if (!mounted) { return; }
    if (state == BlueConnectionState.connected) { _onConnect(devId); }
    else { Navigator.pop(context); } // disconencted
  }
  // get MTU, then discover services
  void _onConnect(devId) async {
    var mtu = await QuickBlue.requestMtu(widget.deviceId, MTU_SIZE_REQUEST);
    for (final key in items.keys) { items[key]?.setDeviceOpts(widget.deviceId, mtu); }
    QuickBlue.discoverServices(devId);
    print("...Discovering...");
  }

  void setNotify(String deviceId, String serviceId, String chara, int delay) {
    Future.delayed(Duration(milliseconds: delay), () { 
      print("Setting notify ($chara)");
      QuickBlue.setNotifiable(deviceId, serviceId, chara, BleInputProperty.notification);
    });
  }
  void setupNotifiers() {
    int delay = 500;
    for (final item in items.values) {
      if (item.notifiable) { setNotify(item.deviceID, item.serviceID, item.characteristicID, delay); delay += 1500; }
    }
    Future.delayed(Duration(milliseconds: delay), () { 
      if (mounted) { setState(() => _bleConnected = true ); } 
    });
  }

  void _handleServiceDiscovery(String deviceId, String serviceId, List<String> characteristics) async {
    print('_handleServiceDiscovery $deviceId, $serviceId, $characteristics');
    if (!mounted) { return; }
    if (serviceId == SERVICE_ID) {
      for (final chara in characteristics) {
        var item = items[chara];
        if (item != null) {
          item.characteristicID = chara;
          item.setServiceID(serviceId);
          if (!item.writeonly) { getQueue.add(item.getValue); }
        }
      }
      // get first value:
      if (getQueue.isNotEmpty) { getQueue.removeAt(0)(); }
    }
  }
  @override
  void dispose() {
    super.dispose();
    QuickBlue.setValueHandler(null);
    QuickBlue.setServiceHandler(null);
    QuickBlue.setConnectionHandler(null);
    QuickBlue.disconnect(widget.deviceId);
    print("...DISPOSE...");
  }
  // TODO: probably want to get initial values first, then set notifiers then enable UI elements...

  void _handleValueChange(String deviceId, String characteristicId, Uint8List value) {
    print('_handleValueChange $deviceId, $characteristicId, ${hex.encode(value)}');
    // oh goodness, I have to make my own data parser??? aaaaa, how is this harder than arduino!? haha.
    // dispatch value
    setState(() => items[characteristicId]?.setValue(value) );
    // if queue, initiate next queued get... otherwise setup notifiers
    if (getQueue.isNotEmpty) { getQueue.removeAt(0)(); }
    else {
      setupNotifiers();
    }
  }

  @override
  Widget build(BuildContext context) {
    var itemkeys = items.keys.toList();
    return Scaffold(
      appBar: AppBar(
        title: const Text('CLOK - Settings'),
        actions: [
          IconButton(
            icon: const Icon(Icons.settings),
            onPressed: () {
              // Navigate to the settings page. If the user leaves and returns
              // to the app after it has been killed while running in the
              // background, the navigation stack is restored.
              Navigator.restorablePushNamed(context, SettingsView.routeName);
            },
          ),
        ],
      ),

      // To work with lists that may contain a large number of items, it’s best
      // to use the ListView.builder constructor.
      //
      // In contrast to the default ListView constructor, which requires
      // building all Widgets up front, the ListView.builder constructor lazily
      // builds Widgets as they’re scrolled into view.
      body:
      Column(
        children: <Widget>[
          Text("Device: ${widget.deviceId}"),
          Expanded(
            child: ListView.builder(
              // Providing a restorationId allows the ListView to restore the
              // scroll position when a user leaves and returns to the app after it
              // has been killed while running in the background.
              restorationId: 'menuListView',
              itemCount: items.length,
              itemBuilder: (BuildContext context, int index) {
                return ListTile(
                    enabled: items[itemkeys[index]]!.display,
                    title: Text(items[itemkeys[index]]!.optionName),
                    subtitle: Text(items[itemkeys[index]]!.optionValue.toString()),
                    onTap: items[itemkeys[index]]!.onTapGen(context)
                  );
              },
            ),
          )
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _bleConnected ? () { items[WIFI_DOSCAN]!.sendData(!items[WIFI_DOSCAN]!.optionValue); } : null ,
        tooltip: items[WIFI_DOSCAN]!.optionValue ? 'Stop Scan' : 'Start Scan',
        child: _bleConnected ? items[WIFI_DOSCAN]!.optionValue ? const Icon(Icons.signal_wifi_statusbar_null): const Icon(Icons.wifi_find) : const Icon(Icons.signal_wifi_bad),
      )
    );
  }
}
