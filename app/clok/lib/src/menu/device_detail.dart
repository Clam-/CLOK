import 'dart:typed_data';
import 'dart:convert';

import 'package:quick_blue/quick_blue.dart';

import '../consts.dart';
import 'package:flutter/material.dart';

import '../settings/settings_view.dart';
import 'simple_control.dart';

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
  final bool _wifiscanning = false;
  bool _bleConnected = false;
  
  // Options List
  final items = {
      // WiFi
      
      // RootCA
      ROOTCA_URL : SimpleControl("RootCA URL", ""),
      // TZ
      TZ_ZONEINFO_URL : SimpleControl("TZ ZoneInfo URL", ""),
      //TZListControl("TZ Select Timezone"),
      TZ_NTP1 : SimpleControl("TZ NTP Server 1", ""),
      TZ_NTP2 : SimpleControl("TZ NTP Server 2", ""),
  };

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
    if (state == BlueConnectionState.connected) {
      // get MTU, then discover services
      _onConnect(devId);      
    }
    if (mounted) { setState(() => _bleConnected = state == BlueConnectionState.connected); }
  }
  void _onConnect(devId) async {
    var mtu = await QuickBlue.requestMtu(widget.deviceId, MTU_SIZE_REQUEST);
    print("MTU: $mtu");
    for (final key in items.keys) { items[key]?.setDeviceOpts(widget.deviceId, mtu); }
    QuickBlue.discoverServices(devId);
    print("...Discovering...");
  }

  void _handleServiceDiscovery(String deviceId, String serviceId, List<String> characteristics) {
    print('_handleServiceDiscovery $deviceId, $serviceId, $characteristics');
    if (!mounted) { return; }
    if (serviceId == SERVICE_ID.toLowerCase()) {
      // store platform specific case of characteristics because... reasons ????
      for (final chara in characteristics) {
        items[chara.toUpperCase()]?.characteristicID = chara;
      }
      // readlater
      print("Found service, do call later...");
      Future.delayed(const Duration(milliseconds: 3500), () async {
        await QuickBlue.readValue(deviceId, serviceId, items[ROOTCA_URL]!.characteristicID);
      });
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

  void _handleValueChange(String deviceId, String characteristicId, Uint8List value) {
    print("HANDLE CHANGE");
    print('_handleValueChange $deviceId, $characteristicId, ${utf8.decode(value)}');
    // oh goodness, I have to make my own data parser??? aaaaa, how is this harder than arduino!? haha.
    setState(() => items[characteristicId]?.setData(utf8.decode(value)) );
  }

  void _toggleScan() {
    if (_wifiscanning) { 
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
                    title: Text(items[itemkeys[index]]!.optionName),
                    onTap: items[itemkeys[index]]!.onTapGen(context)
                  );
              },
            ),
          )
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _bleConnected ? _toggleScan : null ,
        tooltip: _wifiscanning ? 'Stop Scan' : 'Start Scan',
        child: _wifiscanning ? const Icon(Icons.sensors_off): const Icon(Icons.sensors),
      )
    );
  }
}
