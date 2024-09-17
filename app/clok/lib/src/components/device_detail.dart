import 'package:clok/src/components/control_base.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';
import 'package:flutter/material.dart';

import '../consts.dart';
import '../settings/settings_view.dart';
import 'control_string.dart';
import 'control_toggle.dart';
import 'control_wifipicker.dart';

/// Displays detailed information about a SampleItem.
class DeviceDetailsView extends StatefulWidget {
  const DeviceDetailsView({super.key, required this.device});
  static const routeName = '/device';
  final BluetoothDevice device;

@override
  State<DeviceDetailsView> createState() => _DeviceDetailsView();
}

class _DeviceDetailsView extends State<DeviceDetailsView> {

  // state vars
  bool _bleConnected = false;
  List<dynamic Function()> getQueue = [];
  // Options List
  final items = <BaseControl>[] = [];
  BaseControl? wifiScanControl;
  @override
  void initState() {
    super.initState();
    // setup handlers
    if (widget.device.hasGATT) {
      widget.device.connected.listen(_handleConnectionChange);
      widget.device.connect();
    }
  }
  void _handleConnectionChange(bool connected) {
    print('_handleConnectionChange $connected');
    if (!mounted) { return; }
    if (connected) { _onConnect(); }
    else { Navigator.pop(context); } // disconencted
  }
  // get MTU, then discover services
  void _onConnect() async {
    //for (final key in items.keys) { items[key]?.setDeviceOpts(widget.deviceId, mtu); }
    widget.device.discoverServices().then(_handleServiceDiscovery);
    print("...Discovering...");
  }

  void _handleServiceDiscovery(List<BluetoothService> services) async {
    print('_handleServiceDiscovery $services');
    if (!mounted) { return; }
    for (final service in services) {
      if (service.uuid == SERVICE_ID) {
        for (final chara in await service.getCharacteristics()) {
          // ugly.
          switch(chara.uuid) {
            case ROOTCA_URL:
              items.add(StringControl(chara, "RootCA URL", ""));
            case TZ_ZONEINFO_URL:
              items.add(StringControl(chara, "TZ ZoneInfo URL", ""));
            case TZ_NTP1:
              items.add(StringControl(chara, "TZ NTP Server 1", ""));
            case TZ_NTP2:
              items.add(StringControl(chara, "TZ NTP Server 2", ""));
            case WIFI_SSIDS:
              items.add(WiFiPickerControl(chara, "Add WiFi SSID & Key", ""));
            case WIFI_DOSCAN :
              wifiScanControl = ToggleControl(chara, "WiFi Scan", false, display: false);
          }
          if (chara.properties.hasNotify) { chara.startNotifications(); }
        }
      }
    }
  }
  @override
  void dispose() {
    super.dispose();
    widget.device.disconnect();
    print("...DISPOSE...");
  }

  @override
  Widget build(BuildContext context) {
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
      body:
      Column(
        children: <Widget>[
          Text("Device: ${widget.device.name} (${widget.device.id})"),
          Expanded(
            child: ListView.builder(
              // Providing a restorationId allows the ListView to restore the
              // scroll position when a user leaves and returns to the app after it
              // has been killed while running in the background.
              restorationId: 'menuListView',
              itemCount: items.length,
              itemBuilder: (BuildContext context, int index) {
                return ListTile(
                    enabled: items[index].display,
                    title: Text(items[index].optionName),
                    subtitle: Text(items[index].optionValue.toString()),
                    onTap: items[index].onTapGen(context)
                  );
              },
            ),
          )
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _bleConnected ? () { wifiScanControl!.sendData(!wifiScanControl!.optionValue); } : null ,
        tooltip: wifiScanControl!.optionValue ? 'Stop Scan' : 'Start Scan',
        child: _bleConnected ? wifiScanControl!.optionValue ? const Icon(Icons.signal_wifi_statusbar_null): const Icon(Icons.wifi_find) : const Icon(Icons.signal_wifi_bad),
      )
    );
  }
}
