import 'dart:typed_data';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:quick_blue/quick_blue.dart';

import '../settings/settings_view.dart';
import 'simple_control.dart';

/// Displays detailed information about a SampleItem.
class DeviceDetailsView extends StatefulWidget {
  const DeviceDetailsView({super.key, required this.deviceID});
  static const routeName = '/device';
  final String deviceID;

@override
  State<DeviceDetailsView> createState() => _DeviceDetailsView();
}

class _DeviceDetailsView extends State<DeviceDetailsView> {

  // state vars
  bool _WiFiscanning = false;
  bool _BLEConnected = false;
  
  // Options List
  final items = {
      // WiFi
      //WiFiListControl("BAAD0011-5AAD-BAAD-FFFF-5AD5ADBADCLK", "WiFi SSIDs"),
      // RootCA
      "BAAD0031-5AAD-BAAD-FFFF-5AD5ADBADCLK" : SimpleControl("BAAD0031-5AAD-BAAD-FFFF-5AD5ADBADCLK", "RootCA URL", ""),
      // TZ
      "BAAD0051-5AAD-BAAD-FFFF-5AD5ADBADCLK" : SimpleControl("BAAD0051-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ ZoneInfo URL", ""),
      //TZListControl("BAAD0051-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ Select Timezone"),
      "BAAD0056-5AAD-BAAD-FFFF-5AD5ADBADCLK" : SimpleControl("BAAD0056-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ NTP Server 1", ""),
      "BAAD0057-5AAD-BAAD-FFFF-5AD5ADBADCLK" : SimpleControl("BAAD0057-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ NTP Server 2", ""),
  };

  @override
  void initState() {
    super.initState();
    QuickBlue.setConnectionHandler(_handleConnectionChange);
    QuickBlue.setValueHandler(_handleValueChange);
  }
  @override
  void dispose() {
    super.dispose();
    QuickBlue.setValueHandler(null);
    QuickBlue.setServiceHandler(null);
    QuickBlue.setConnectionHandler(null);
  }

  void _handleConnectionChange(String deviceId, BlueConnectionState state) {
    print('_handleConnectionChange $deviceId, $state');
    if (state == BlueConnectionState.connected) {
      // do some BLE setup... setup notifiers, etc...
      //setup notifiers...
      //QuickBlue.setNotifiable(deviceId, serviceId, characteristicId, true);
    }
    setState(() => _BLEConnected = state == BlueConnectionState.connected);
  }
  void _handleValueChange(String deviceId, String characteristicId, Uint8List value) {
    print('_handleValueChange $deviceId, $characteristicId, ${utf8.decode(value)}');
    // oh goodness, I have to make my own data parser??? aaaaa, how is this harder than arduino!? haha.

  }

  void _toggleScan() {
    if (_WiFiscanning) { 
      // stop scanning

     }
    else { 
      //start scanning

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
          Text("Device: ${widget.deviceID}"),
          Expanded(
            child: ListView.builder(
              // Providing a restorationId allows the ListView to restore the
              // scroll position when a user leaves and returns to the app after it
              // has been killed while running in the background.
              restorationId: 'menuListView',
              itemCount: items.length,
              itemBuilder: (BuildContext context, int index) {
                return ListTile(
                    title: Text(items[itemkeys[index]]!.optionName),
                    onTap: items[itemkeys[index]]!.onTapGen(context)
                  );
              },
            ),
          )
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _BLEConnected ? _toggleScan : null ,
        tooltip: _WiFiscanning ? 'Stop Scan' : 'Start Scan',
        child: _WiFiscanning ? const Icon(Icons.sensors_off): const Icon(Icons.sensors),
      )
    );
  }
}
